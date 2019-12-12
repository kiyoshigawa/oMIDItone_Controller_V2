/*
This is a library designed to work with the Teensy MIDI libraries to control
a MIDI device from the Teensy.

The library is designed to keep track of all incoming MIDI messages and maintain
a 'state-of-the-synth' controller object that has current information based on
the sum total of all MIDI messages received to that point.

Copyright 2019 - kiyoshigawa - tim@twa.ninja

*/
/*
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <MIDIController.h>

//init the hardware MIDI:
MIDI_CREATE_INSTANCE(HARDWARE_MIDI_TYPE, HARDWARE_MIDI_INTERFACE, MIDI);

//this an array of function pointers to the MIDI CC handler functions.
//they can be overridden from the default values shown here by setting a new 
//function pointer using the assign_midi_cc_handler() function.
typedef void (*cc_handler_pointer)(uint8_t channel, uint8_t cc_value);
cc_handler_pointer midi_cc_handler_function_array[NUM_MIDI_CC_TYPES];

MIDIController::MIDIController(void)
{
	//init the pointers for the MIDI CC handlers to null on creation
	for(int i=0; i<NUM_MIDI_CC_TYPES; i++){
		midi_cc_handler_function_array[i] = NULL;
	}
	//put this into a function so that it can also be called from MIDI CC 121.
	reset_to_default();
}

/* ----- PUBLIC FUNCTIONS BELOW ----- */

void MIDIController::init(void)
{
	//init the hardware MIDI interface
	MIDI.begin(MIDI_CHANNEL_OMNI);
	//and init the USB MIDI interface
	usbMIDI.begin();
}

void MIDIController::update(void)
{
	//this takes any new raw MIDI data bytes and calls the appropriate handler functions to update the controller.
	process_MIDI();
}

void MIDIController::assign_midi_cc_handler(uint8_t cc_number, cc_handler_pointer function_pointer)
{
	midi_cc_handler_function_array[cc_number] = function_pointer;
}

void MIDIController:: set_omni_off_receive_channel(uint8_t channel)
{
	//validity check:
	if(channel < NUM_MIDI_CHANNELS){
		omni_off_receive_channel = channel;
	} else {
		//set to default on invalid input:
		omni_off_receive_channel = MIDI_DEFAULT_RECEIVE_CHANNEL;
	}
}
bool MIDIController::note_was_added(void)
{
	if(new_note_added){
		//reset to false whenever this is called to avoid getting stuck in a tune request loop
		new_note_added = false;
		return true;
	}
	else{
		return false;
	}
}

bool MIDIController::note_was_changed(void)
{
	if(new_note_changed){
		//reset to false whenever this is called to avoid getting stuck in a tune request loop
		new_note_changed = false;
		return true;
	}
	else{
		return false;
	}
}

bool MIDIController::note_was_removed(void)
{
	if(new_note_removed){
		//reset to false whenever this is called to avoid getting stuck in a tune request loop
		new_note_removed = false;
		return true;
	}
	else{
		return false;
	}
}

bool MIDIController::cc_was_changed(void)
{
	if(new_cc_message){
		//reset to false whenever this is called to avoid getting stuck in a tune request loop
		new_cc_message = false;
		return true;
	}
	else{
		return false;
	}
}

bool MIDIController::program_mode_was_changed(void)
{
	if(new_program_mode){
		//reset to false whenever this is called to avoid getting stuck in a tune request loop
		new_program_mode = false;
		return true;
	}
	else{
		return false;
	}
}

bool MIDIController::tune_request_was_received(void){
	if(new_tune_request){
		//reset to false whenever this is called to avoid getting stuck in a tune request loop
		new_tune_request = false;
		return true;
	}
	else{
		return false;
	}
}

bool MIDIController::system_reset_request_was_received(void)
{
	if(new_system_reset_request){
		//reset to false whenever this is called to avoid getting stuck in a tune request loop
		new_system_reset_request = false;
		return true;
	}
	else{
		return false;
	}
}

bool MIDIController::is_local_control_enabled(void)
{
	return local_control_is_enabled;
}
/* ----- END PUBLIC FUNCTIONS ----- */
/* ----- PRIVATE FUNCTIONS BELOW ----- */

void MIDIController::process_MIDI(void)
{
	if(MIDI.read()){
		uint8_t type = MIDI.getType();
		uint8_t channel = MIDI.getChannel();
		uint8_t data_1 = MIDI.getData1();
		uint8_t data_2 = MIDI.getData2();
		assign_MIDI_handlers(type, channel, data_1, data_2);
	}
	if(usbMIDI.read()){
		uint8_t type = usbMIDI.getType();
		uint8_t channel = usbMIDI.getChannel();
		uint8_t data_1 = usbMIDI.getData1();
		uint8_t data_2 = usbMIDI.getData2();
		assign_MIDI_handlers(type, channel, data_1, data_2);
	}
}

void MIDIController::assign_MIDI_handlers(uint8_t type, uint8_t channel, uint8_t data_1, uint8_t data_2)
{
	//return without taking any action if the channel is wrong when omni mode is off
	if(!omni_mode_is_enabled){
		if(channel != omni_off_receive_channel){
			return;
		}
	}
	switch(type){
	case usbMIDI.NoteOff:
		handle_note_off(channel, data_1, data_2);
		break;
	case usbMIDI.NoteOn:
		handle_note_on(channel, data_1, data_2);
		break;
	case usbMIDI.AfterTouchPoly:
		handle_aftertouch_poly(channel, data_1, data_2);
		break;
	case usbMIDI.ControlChange:
		handle_control_change(channel, data_1, data_2);
		break;
	case usbMIDI.ProgramChange:
		handle_program_change(channel, data_1);
		break;
	case usbMIDI.AfterTouchChannel:
		handle_aftertouch_channel(channel, data_1);
		break;
	case usbMIDI.PitchBend:
		handle_pitch_bend(channel, ((data_2<<7) + data_1 - 8192));
		break;
	case usbMIDI.TuneRequest:
		handle_tune_request();
		break;
	case usbMIDI.SystemReset:
		handle_system_reset();
		break;
	default:
		#ifdef MIDI_DEBUG_IGNORED
			Serial.print("MIDI Message of type ");
			Serial.print(type);
			Serial.println(" ignored.");
		#endif
		break;
	}
}

void MIDIController::handle_note_on(uint8_t channel, uint8_t note, uint8_t velocity)
{	
	add_note(channel, note, velocity);
	new_note_added = true;
}

void MIDIController::handle_note_off(uint8_t channel, uint8_t note, uint8_t velocity)
{
	rm_note(channel, note);
	new_note_removed = true;
}

void MIDIController::handle_pitch_bend(uint8_t channel, int16_t pitch)
{
	//set the channel pitch bend value
	current_pitch_bends[channel] = pitch;
	//If any current notes are on that channel, update their frequencies:
	for(int i=0; i<num_current_notes; i++){
		if(current_notes[i].channel == channel){
			//the pitch bend will be calculated automatically by this function based on the updated current_pitch_bends[channel] value
			current_notes[i].freq = calculate_note_frequency(channel, current_notes[i].note);
		}
	}
	new_note_changed = true;
	#ifdef MIDI_DEBUG_PITCH_BEND
		Serial.print("Pitch Bend: ");
		Serial.print(channel);
		Serial.print(":");
		Serial.println(pitch);
	#endif
}

void MIDIController::handle_aftertouch_channel(uint8_t channel, uint8_t pressure)
{
	//set the channel aftertouch value
	current_channel_aftertouches[channel] = pressure;
	//if any current notes are on that channel, update their velocity to be pressure
	for(int i=0; i<num_current_notes; i++){
		if(current_notes[i].channel == channel){
			//the velocity is set to pressure for all notes on the channel
			current_notes[i].velocity = pressure;
		}
	}
	new_note_changed = true;
	#ifdef MIDI_DEBUG_AFTERTOUCH
		Serial.print("Channel Aftertouch: ");
		Serial.print(channel);
		Serial.print(":");
		Serial.println(pressure);
	#endif
}

void MIDIController::handle_aftertouch_poly(uint8_t channel, uint8_t note, uint8_t velocity)
{
	//this only effects currently playing notes, and is not stored anywhere.
	for(int i=0; i<num_current_notes; i++){
		if(current_notes[i].channel == channel && current_notes[i].note == note){
			current_notes[i].velocity = velocity;
		}
	}
	new_note_changed = true;
	#ifdef MIDI_DEBUG_AFTERTOUCH
		Serial.print("Poly Aftertouch: ");
		Serial.print(channel);
		Serial.print(":");
		Serial.print(note);
		Serial.print(":");
		Serial.println(velocity);
	#endif
}

void MIDIController::handle_program_change(uint8_t channel, uint8_t program)
{
	//This just sets the channel's program number. It's up to whatever's using the controller to handle it.
	current_program_modes[channel] = program;
	new_program_mode = true;
	#ifdef MIDI_DEBUG_SYSTEM
		Serial.print("Program Change: ");
		Serial.print(channel);
		Serial.print(":");
		Serial.println(program);
	#endif
}

void MIDIController::handle_control_change(uint8_t channel, uint8_t cc_type, uint8_t cc_value)
{
	//this will update the current_cc_values array to match the most recently received data byte
	current_cc_values[channel][cc_type] = cc_value;
	new_cc_message = true;
	//now we will call the user-defined handler function if it has been defined.
	//this switch will check the cases with defaults, specifically CC6, CC38, CC96-101, and CC120-127.
	switch(cc_type){
	//these cases are all related to RPN value manipulation, so if any of them 
	//have handlers, we need to call those instead of handle_rpn_change
	case MIDI_CC::rpn_data_entry_msb:
	case MIDI_CC::rpn_data_entry_lsb:
	case MIDI_CC::data_increment:
	case MIDI_CC::data_decrement:
	case MIDI_CC::rpn_lsb:
	case MIDI_CC::rpn_msb:
		//TIM: this is broken, you need to fix it so it will work correctly.
		if(midi_cc_handler_function_array[cc_type] != NULL){
			midi_cc_handler_function_array[cc_type](channel, cc_value);
		} else {
			handle_rpn_change(
				channel, 
				current_cc_values[channel][MIDI_CC::rpn_msb],
				current_cc_values[channel][MIDI_CC::rpn_lsb],
				current_cc_values[channel][MIDI_CC::rpn_data_entry_msb],
				current_cc_values[channel][MIDI_CC::rpn_data_entry_lsb]);
		}
		#ifdef MIDI_DEBUG_CC
			Serial.println("RPN Received...");
		#endif
		break;
	//as far as this controller is concerned, sound off and all notes off work the same.
	case MIDI_CC::all_sound_off:
	case MIDI_CC::all_notes_off:
		if(midi_cc_handler_function_array[cc_type] != NULL){
			#ifdef MIDI_DEBUG_CC
				Serial.print("CC: ");
				Serial.print(channel);
				Serial.print(":");
				Serial.print(cc_type);
				Serial.print(":");
				Serial.print(cc_value);
				Serial.println(" handled by user function.");
			#endif
			midi_cc_handler_function_array[cc_type](channel, cc_value);
			break;
		}
		//set the number of current notes to 0 immediately
		num_current_notes = 0;
		//notify the controller user that notes have ended so they can take action
		new_note_removed = true;
		#ifdef MIDI_DEBUG_CC
			Serial.println("All notes/sound off received.");
		#endif
		break;
	//this is essentially the same as a freshly init()ed controller.
	case MIDI_CC::reset_all_controllers:
		if(midi_cc_handler_function_array[cc_type] != NULL){
			#ifdef MIDI_DEBUG_CC
				Serial.print("CC: ");
				Serial.print(channel);
				Serial.print(":");
				Serial.print(cc_type);
				Serial.print(":");
				Serial.print(cc_value);
				Serial.println(" handled by user function.");
			#endif
			midi_cc_handler_function_array[cc_type](channel, cc_value);
			break;
		}
		//reset all values to defaults
		reset_to_default();
		//notify te controller user that things have changed so they can take action
		new_note_removed = true;
		new_program_mode = true;
		new_cc_message = true;
		#ifdef MIDI_DEBUG_CC
			Serial.println("Controller reset to defaults.");
		#endif
		break;
	//this one needs to be handled by the user of the controller, so we've put 
	//in the is_local_control_enabled() function for people who want to use this
	case MIDI_CC::local_control_on_off:
		if(midi_cc_handler_function_array[cc_type] != NULL){
			#ifdef MIDI_DEBUG_CC
				Serial.print("CC: ");
				Serial.print(channel);
				Serial.print(":");
				Serial.print(cc_type);
				Serial.print(":");
				Serial.print(cc_value);
				Serial.println(" handled by user function.");
			#endif
			midi_cc_handler_function_array[cc_type](channel, cc_value);
			break;
		}
		if(cc_value == 0){
			local_control_is_enabled = false;
		} else if(cc_value == 127){
			local_control_is_enabled = true;
		}
		//other values are undefined, do nothing except in these two cases.
		#ifdef MIDI_DEBUG_CC
			Serial.print("Local control ");
			if(local_control_is_enabled){Serial.print("enabled");}
			else{Serial.print("disabled");}
			Serial.println(".");
		#endif
		break;
	case MIDI_CC::omni_mode_off:
		if(midi_cc_handler_function_array[cc_type] != NULL){
			#ifdef MIDI_DEBUG_CC
				Serial.print("CC: ");
				Serial.print(channel);
				Serial.print(":");
				Serial.print(cc_type);
				Serial.print(":");
				Serial.print(cc_value);
				Serial.println(" handled by user function.");
			#endif
			midi_cc_handler_function_array[cc_type](channel, cc_value);
			break;
		}
		omni_mode_is_enabled = false;
		//set the number of current notes to 0 immediately
		num_current_notes = 0;
		//notify the controller user that notes have ended so they can take action
		new_note_removed = true;
		#ifdef MIDI_DEBUG_CC
			Serial.print("Omni Mode off, listening on channel: ");
			Serial.println(omni_off_receive_channel);
		#endif
		break;
	case MIDI_CC::omni_mode_on:
		if(midi_cc_handler_function_array[cc_type] != NULL){
			#ifdef MIDI_DEBUG_CC
				Serial.print("CC: ");
				Serial.print(channel);
				Serial.print(":");
				Serial.print(cc_type);
				Serial.print(":");
				Serial.print(cc_value);
				Serial.println(" handled by user function.");
			#endif
			midi_cc_handler_function_array[cc_type](channel, cc_value);
			break;
		}
		omni_mode_is_enabled = true;
		//set the number of current notes to 0 immediately
		num_current_notes = 0;
		//notify the controller user that notes have ended so they can take action
		new_note_removed = true;
		#ifdef MIDI_DEBUG_CC
			Serial.println("Omni Mode on.");
		#endif
		break;
	case MIDI_CC::mono_mode_on:
		if(midi_cc_handler_function_array[cc_type] != NULL){
			#ifdef MIDI_DEBUG_CC
				Serial.print("CC: ");
				Serial.print(channel);
				Serial.print(":");
				Serial.print(cc_type);
				Serial.print(":");
				Serial.print(cc_value);
				Serial.println(" handled by user function.");
			#endif
			midi_cc_handler_function_array[cc_type](channel, cc_value);
			break;
		}
		poly_mode_is_enabled = false;
		num_mono_voices = cc_value;
		//set the number of current notes to 0 immediately
		num_current_notes = 0;
		//notify the controller user that notes have ended so they can take action
		new_note_removed = true;
		#ifdef MIDI_DEBUG_CC
			Serial.print("Mono Mode on with ");
			Serial.print(cc_value);
			Serial.println(" voices.");
		#endif
		break;
	case MIDI_CC::poly_mode_on:
		if(midi_cc_handler_function_array[cc_type] != NULL){
			#ifdef MIDI_DEBUG_CC
				Serial.print("CC: ");
				Serial.print(channel);
				Serial.print(":");
				Serial.print(cc_type);
				Serial.print(":");
				Serial.print(cc_value);
				Serial.println(" handled by user function.");
			#endif
			midi_cc_handler_function_array[cc_type](channel, cc_value);
			break;
		}
		poly_mode_is_enabled = true;
		//set the number of current notes to 0 immediately
		num_current_notes = 0;
		//notify the controller user that notes have ended so they can take action
		new_note_removed = true;
		#ifdef MIDI_DEBUG_CC
			Serial.println("Poly mode on.");
		#endif
		break;
	default:
		if(midi_cc_handler_function_array[cc_type] != NULL){
			#ifdef MIDI_DEBUG_CC
				Serial.print("CC: ");
				Serial.print(channel);
				Serial.print(":");
				Serial.print(cc_type);
				Serial.print(":");
				Serial.print(cc_value);
				Serial.println(" handled by user function.");
			#endif
			midi_cc_handler_function_array[cc_type](channel, cc_value);
			break;
		}
		#ifdef MIDI_DEBUG_CC
			Serial.print("CC: ");
			Serial.print(channel);
			Serial.print(":");
			Serial.print(cc_type);
			Serial.print(":");
			Serial.print(cc_value);
			Serial.println("not assigned, no action taken.");
		#endif
	}
}

void MIDIController::handle_rpn_change(uint8_t channel, uint8_t rpn_msb, uint8_t rpn_lsb, uint8_t cc6_value, uint8_t cc38_value)
{

}

void MIDIController::handle_tune_request()
{
	//this sets a flag that can be checked by whatever's using the controller to call a tune change
	new_tune_request = true;
	#ifdef MIDI_DEBUG_SYSTEM
		Serial.print("Tune Request Received.");
	#endif
}

void MIDIController::handle_system_reset()
{
	new_system_reset_request = true;
	#ifdef MIDI_DEBUG_SYSTEM
		Serial.print("System reset message received - restarting...");
	#endif
}

void MIDIController::reset_to_default(void)
{
	num_current_notes = 0;
	new_tune_request = false;
	poly_mode_is_enabled = MIDI_DEFAULT_POLY_STATE;
	omni_mode_is_enabled = MIDI_DEFAULT_OMNI_STATE;
	num_mono_voices = MIDI_DEFAULT_MONO_VOICES;
	omni_off_receive_channel = MIDI_DEFAULT_RECEIVE_CHANNEL;
	local_control_is_enabled = MIDI_DEFAULT_LOCAL_CONTROL_STATE;
	//init things in arrays NUM_MIDI_CHANNELS long:
	for(int c=0; c<NUM_MIDI_CHANNELS; c++){
		current_channel_aftertouches[c] = NO_NOTE; //this doesn't mater, it's just a placeholder. It will only effect currently playing notes
		current_pitch_bends[c] = 0;
		current_program_modes[c] = 0; //0 is the default program mode per the MIDI spec
		//init the CC array to a default of NO_NOTE for all CC to indicate that it has not yet been set
		for(int i=0; i<NUM_MIDI_CC_TYPES; i++){
			current_cc_values[c][1] = NO_NOTE;
		}
		//init the RPN 0 Array:
		for(int i=0; i<MIDI_NUM_RPN_0; i++){
			rpn_0_array[c][i] = MIDI_RPN_NULL;
		}
		//init the RPN 3D Array:
		for(int i=0; i<MIDI_NUM_RPN_3D; i++){
			rpn_3d_array[c][i] = MIDI_RPN_NULL;
		}
	}
}

void MIDIController::add_note(uint8_t channel, uint8_t note, uint8_t velocity){
	//first verify that the note values make sense
	uint8_t max_notes = MAX_CONCURRENT_MIDI_NOTES;
	if(!poly_mode_is_enabled && num_mono_voices != 0){
		//set the max number of notes based on the mono mode settings
		max_notes = num_mono_voices;
	}
	if(num_current_notes < max_notes && note < NUM_MIDI_NOTES){
		int8_t note_position = check_note(channel, note);
		//if the note isn't already in the array, put it at the end of the array.
		if(note_position == NOT_IN_ARRAY){
			num_current_notes++;
			current_notes[num_current_notes-1].channel = channel;
			current_notes[num_current_notes-1].note = note;
			current_notes[num_current_notes-1].velocity = velocity;
			current_notes[num_current_notes-1].freq = calculate_note_frequency(channel, note);
		} else {
			//if it is in the array, put the note at the end and bend everything back down to where the note used to be.
			for(int i=note_position; i<num_current_notes; i++){
				current_notes[i] = current_notes[i+1];
			}
			current_notes[num_current_notes-1].channel = channel;
			current_notes[num_current_notes-1].note = note;
			current_notes[num_current_notes-1].velocity = velocity;
			current_notes[num_current_notes-1].freq = calculate_note_frequency(channel, note);
		}
		//if note debug is on, this will print the current note array every time a note is added or removed
		print_current_notes();
	} else {
		int8_t note_position = check_note(channel, note);
		//if the note isn't already in the array, put it at the end of the array.
		if(note_position == NOT_IN_ARRAY){
			//we need to shift every note down and put the new note on the end in this case.
			//the end result is that the oldest note is gone forever, and only the most current max_notes notes remain in the array.
			for(int i=0; i<num_current_notes; i++){
				current_notes[i] = current_notes[i+1];
			}
			current_notes[num_current_notes-1].channel = channel;
			current_notes[num_current_notes-1].note = note;
			current_notes[num_current_notes-1].velocity = velocity;
			current_notes[num_current_notes-1].freq = calculate_note_frequency(channel, note);
			//notify that a note was removed from the array so it can be stopped.
			new_note_removed = true;
		}
		//if note debug is on, this will print the current note array every time a note is added or removed
		print_current_notes();
	}
}

void MIDIController::rm_note(uint8_t channel, uint8_t note){
	int8_t note_position = check_note(channel, note);
	//if the note is in the note array, remove it and bend down any other notes.
	if(note_position != NOT_IN_ARRAY){
		for(int i=note_position; i<num_current_notes; i++){
			current_notes[i] = current_notes[i+1];
		}
		num_current_notes--;
		//make sure it doesn't remove more notes than we have
		if(num_current_notes < 0){
			num_current_notes = 0;
		}
		//if note debug is on, this will print the current note array every time a note is added or removed
		print_current_notes();
	} else {
		//Do nothing
	}
}

int8_t MIDIController::check_note(uint8_t channel, uint8_t note){
	//Iterate through the array, and return the note position if it is found.
	if(num_current_notes == 0){
		return NOT_IN_ARRAY;
	} else {
		for(int i=0; i<num_current_notes; i++){
			if(current_notes[i].channel == channel && current_notes[i].note == note){
				return i;
			}
		}
		//if no note is found, return NOT_IN_ARRAY;
		return NOT_IN_ARRAY;
	}
}

uint32_t MIDIController::calculate_note_frequency(uint8_t channel, uint8_t note){
	if(current_pitch_bends[channel] == 0){
		return midi_freqs[note];
	}
	else{
		//TIM: Make pitch bending work here:
		//For now, just returning unbent note values, because I want something to play
		return midi_freqs[note];
	}
}

void MIDIController::print_current_notes(void){
	#ifdef MIDI_NOTE_DEBUG
		if(num_current_notes > 0){
			Serial.print("Notes: [");
			for(int i=0; i<num_current_notes-1; i++){
				Serial.print(current_notes[i].channel);
				Serial.print(":");
				Serial.print(current_notes[i].note);
				#ifdef MIDI_NOTE_DEBUG_VERBOSE
					Serial.print(":");
					Serial.print(current_notes[i].velocity);
					Serial.print(":");
					Serial.print(current_notes[i].freq);
				#endif
				Serial.print(", ");
			}
			Serial.print(current_notes[num_current_notes-1].channel);
			Serial.print(":");
			Serial.print(current_notes[num_current_notes-1].note);
			#ifdef MIDI_NOTE_DEBUG_VERBOSE
				Serial.print(":");
				Serial.print(current_notes[num_current_notes-1].velocity);
				Serial.print(":");
				Serial.print(current_notes[num_current_notes-1].freq);
			#endif
			Serial.println("]");
		} else {
			Serial.println("Notes: []");
		}
	#endif
}

/* ----- END PRIVATE FUNCTIONS ----- */
/*

//Below are functions taken out of the oMIDItone class that need to be reintegrated with the controller class:

uint32_t oMIDItone::pitch_adjusted_frequency(uint8_t note, int16_t pitch_bend){
	//First the trivial case of CENTER_PITCH_BEND:
	if(pitch_bend == CENTER_PITCH_BEND){
		return midi_freqs[note];
	} else {
		//this takes the pitch_bend and maps it to the corresponding number of cents
		int32_t pitch_bend_offset = map(pitch_bend, -8192, 8192, -max_pitch_bend_offset, max_pitch_bend_offset);
		//figure out how manu semitones away the pitch_bend_offset is:
		//the +50/-50 is to make it round correctly by offsetting halfway to the next integer and letting c truncate in the right direction for me
		int32_t num_semitones_bent;
		if(pitch_bend_offset < 0){
			num_semitones_bent = (pitch_bend_offset-50)/100;
		} else {
			num_semitones_bent = (pitch_bend_offset+50)/100;
		}
		//this calculates how many cents past the nearest note frequency we need to bend
		int32_t leftover_cents = pitch_bend_offset - (num_semitones_bent*100);
		//this calls out the nearest note that we will use for the cent offset so that it's within range of the cent_frequency_ratios table
		uint8_t adjusted_note = note + num_semitones_bent;

		//the inflated_ratio is the frequency times the frequency adjustment ratio 2^(cents/1200) times 1,000,000.
		//this is multiplied by 1,000,000 so we can use integer math instead of floating point math to make things speedier
		uint64_t inflated_ratio = (uint64_t)midi_freqs[adjusted_note]*(uint64_t)cent_frequency_ratios[(uint32_t)(leftover_cents+100)];
		//and we need to divide by 1,000,000 to cancel out the inflation above to get the actual frequency we desire.
		uint32_t adjusted_frequency = inflated_ratio/CENT_FREQUENCY_RATIO_MULTIPLIER;

		return adjusted_frequency;
	}
}

void oMIDItone::set_pitch_bend(int16_t pitch_bend_value, uint8_t pitch_bend_channel){
		current_oMIDItone_pitch_bend[pitch_bend_channel] = pitch_bend_value;
		//update the note if the pitch chift is on the same channel as the currently_playing_note:
		if(pitch_bend_channel == current_channel){
			current_desired_freq = pitch_adjusted_freq(current_note, current_oMIDItone_pitch_bend[current_channel]);
			//set the averaging function to the new frequency in anticipation of the change:
			for(int i=0; i<NUM_FREQ_READINGS; i++){
				recent_freqs[i] = current_desired_freq;
			}
			//set the current_resistance to a value that was previously measured as close to the desired note's frequency.
			current_resistance = freq_to_resistance(current_desired_freq);
		}
		update();
}


void oMIDItone::set_max_pitch_bend(uint8_t semitones, uint8_t cents){
	//do some error checking to make sure that this is vaid
	if(semitones > MAX_PITCH_BEND_SEMITONES){
		semitones = MAX_PITCH_BEND_SEMITONES;
	} else if(semitones <= 0){
		semitones = 1;
	}
	if(cents > MAX_PITCH_BEND_CENTS){
		cents = MAX_PITCH_BEND_CENTS;
	} else if(cents <= 0){
		cents = 1;
	}
	//this is the max offset from the center in cents.
	//Pitch bend can go from note-max_offset to note+max_offset.
	max_pitch_bend_offset = semitones*100 + cents;
}


//this function moves the head in question to the end of the head_order_array, and shuffles the remaining heads down into its place.
void pending_head_order_to_end(uint8_t head_number){
	//don't need to iterate to the last one, because if the head_number is in the last position already, we're good
	for(int i=0; i<NUM_OMIDITONES-1; i++){
		if(pending_head_order_array[i] == head_number){
			//set the current position to the next head in line
			pending_head_order_array[i] = pending_head_order_array[i+1];
			//and move the head in question down the line until it's in the last place
			pending_head_order_array[i+1] = head_number;
		}
	}
}

*/