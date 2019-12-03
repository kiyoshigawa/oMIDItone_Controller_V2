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

//this is just a quick function to print the note array with some formatting:
void print_current_note_array(){
	#ifdef NOTE_DEBUG
		if(num_current_notes > 0){
			Serial.print("Notes: [");
			if(num_current_notes > 0){
				for(int i=0; i<num_current_notes-1; i++){
					Serial.print(current_note_array[i]);
					Serial.print(", ");
				}
			}
			Serial.print(current_note_array[num_current_notes-1]);
			Serial.println("]");
		} else {
			Serial.println("Notes: []");
		}
	#endif
}

//This will check to see if a note is in the current_note_array and return the position of the note if it finds one, or return NO_NOTE if it does not.
int check_note(uint8_t note){
	//Iterate through the array, and return the note position if it is found.
	if(num_current_notes == 0){
		return NO_NOTE;
	} else {
		for(int i=0; i<num_current_notes; i++){
			if(current_note_array[i] == note){
				return i;
			}
		}
		//if no note is found, return NO_NOTE;
		return NO_NOTE;
	}
}

//This will add a new note to the end of the current_note_array, or relocate the note to the end if it is already in the array.
//It will also add a velocity value corresponding to that note to the current_velocity_array.
void add_note(uint8_t note, uint8_t velocity, uint8_t channel){
	uint8_t note_position = check_note(note);
	//if the note isn't already in the array, put it at the end of the array.
	if(note_position == NO_NOTE){
		current_note_array[num_current_notes] = note;
		current_velocity_array[num_current_notes] = velocity;
		current_channel_array[num_current_notes] = channel;
		num_current_notes++;
	} else {
		//if it is in the array, put the note at the end and bend everything back down to where the note used to be.
		for(int i=note_position; i<num_current_notes; i++){
			current_note_array[i] = current_note_array[i+1];
			current_velocity_array[i] = current_velocity_array[i+1];
			current_channel_array[i] = current_channel_array[i+1];
		}
		current_note_array[num_current_notes-1] = note;
		current_velocity_array[num_current_notes-1] = velocity;
		current_channel_array[num_current_notes-1] = channel;
	}
}

//If a note is in the current_note_array, this will remove it, and the corresponding velocity will be removed from the current_velocity_array
//and then it will bend the rest of the values down to fill in the gap.
void remove_note(uint8_t note){
	uint8_t note_position = check_note(note);
	//if the note is in the note array, remove it and bend down any other notes.
	if(note_position != NO_NOTE){
		for(int i=note_position; i<num_current_notes; i++){
			current_note_array[i] = current_note_array[i+1];
			current_velocity_array[i] = current_velocity_array[i+1];
			current_channel_array[i] = current_channel_array[i+1];
		}
		num_current_notes--;
	} else {
		//Do nothing
	}
	//need to get the head to immediately stop playing the note as well:
	for(int i=0; i<NUM_OMIDITONES; i++){
		if(oms[i].currently_playing_note() == note){
			oms[i].note_off(note);
		}
	}

	//and finally change the head_order_array to match the pending_head_order_array when all notes are off
	if(num_current_notes <= 0){
		for(int i=0; i<NUM_OMIDITONES; i++){
			head_order_array[i] = pending_head_order_array[i];
		}
	}
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