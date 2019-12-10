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

#ifndef MIDIController_h
#define MIDIController_h

#include <Arduino.h>
#include <MIDI.h>
#include <usb_midi.h>

//uncomment this to get debug messages about ignored MIDI messages
//#define MIDI_DEBUG_IGNORED

//uncomment this to get debug messages about MIDI Aftertouch
#define MIDI_DEBUG_AFTERTOUCH

//uncomment this to get debug messages about MIDI Pitch Bends
#define MIDI_DEBUG_PITCH_BEND

//uncomment this to get debug messages about MIDI program and system messages
#define MIDI_DEBUG_SYSTEM

//uncomment this to get debug messages about MIDI CC messages
#define MIDI_DEBUG_CC

//uncomment this to get printouts of the current note array every time a note is added or removed
#define MIDI_NOTE_DEBUG

//uncomment this to add more info to the MIDI note debug messages, including velocity and inverted frequency
#define MIDI_NOTE_DEBUG_VERBOSE

//defaults used when creating the hardware MIDI interface
//see teensy MIDI.h library documentation for more info
#define HARDWARE_MIDI_TYPE HardwareSerial
#define HARDWARE_MIDI_INTERFACE Serial1

//This is a non-valid note number for MIDI to designate no note should be played.
#define NO_NOTE 128

//This is a non-valid channel number for MIDI to designate no current channel.
#define NO_CHANNEL 17

//this is a non-valid array position used to denote that a note was not found in an array
#define NOT_IN_ARRAY -1

//This is how many MIDI notes there are. It should always be 128
#define NUM_MIDI_NOTES 128

//This is how many MIDI CC message types there are. It should always be 128
#define NUM_MIDI_CC_TYPES 128

//This is the number of MIDI channels. Should always be 16, unless MIDI changed sometime in the last decade.
#define NUM_MIDI_CHANNELS 16

//this is the most concurrent midi notes that the controler will keep track of at any given time:
#define MAX_CONCURRENT_MIDI_NOTES 20

//these are the default values for pitch bend range:
#define DEFAULT_MIDI_PITCH_BEND_SEMITONES 2
#define DEFAULT_MIDI_PITCH_BEND_CENTS 0
#define MAX_PITCH_BEND_SEMITONES 24
#define MAX_PITCH_BEND_CENTS 99

//this is the center value for pitch bending - if the pitch_bend vlaue is set to this, it will play the note without any pitch bending.
//Teensy MIDI sends a number from -8192 to 8191, so I am using 0 as the center here. Typical MIDI center is 8192, adjust as needed.
#define CENTER_PITCH_BEND 0

//this is how much I multiplied the cent frequency ratios by to make things work with integer math
#define CENT_FREQUENCY_RATIO_MULTIPLIER 1000000

//This is an array of MIDI notes and the frequency they correspond to. Turns out it is not needed.
//const float midi_Hz_freqs[NUM_MIDI_NOTES] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.5, 25.957, 27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963, 3322.438, 3520, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040, 7458.62, 7902.133, 8372.018, 8869.844, 9397.273, 9956.063, 10548.08, 11175.3, 11839.82, 12543.85};

//This is an array that has converted the midi_freqs_Hz array into an array of integers representing us between rising edges for the note frequencies
const uint32_t midi_freqs[NUM_MIDI_NOTES] = {122309, 115446, 108968, 102848, 97077, 91633, 86490, 81632, 77053, 72727, 68643, 64792, 61154, 57723, 54484, 51427, 48538, 45814, 43243, 40816, 38525, 36363, 34322, 32396, 30578, 28861, 27242, 25712, 24270, 22907, 21622, 20408, 19262, 18181, 17161, 16198, 15289, 14430, 13621, 12856, 12134, 11453, 10810, 10204, 9631, 9090, 8580, 8099, 7644, 7215, 6810, 6428, 6067, 5726, 5405, 5102, 4815, 4545, 4290, 4049, 3822, 3607, 3405, 3214, 3033, 2863, 2702, 2551, 2407, 2272, 2145, 2024, 1911, 1803, 1702, 1607, 1516, 1431, 1351, 1275, 1203, 1136, 1072, 1012, 955, 901, 851, 803, 758, 715, 675, 637, 601, 568, 536, 506, 477, 450, 425, 401, 379, 357, 337, 318, 300, 284, 268, 253, 238, 225, 212, 200, 189, 178, 168, 159, 150, 142, 134, 126, 119, 112, 106, 100, 94, 89, 84, 79};

//this is the ratio that frequencies change from -99 cents to 99 cents, multiplied by CENT_FREQUENCY_RATIO_MULTIPLIER, in this case 1,000,000.
const uint32_t cent_frequency_ratios[199] = {1058851, 1058240, 1057629, 1057018, 1056408, 1055798, 1055188, 1054579, 1053970, 1053361, 1052753, 1052145, 1051537, 1050930, 1050323, 1049717, 1049111, 1048505, 1047899, 1047294, 1046689, 1046085, 1045481, 1044877, 1044274, 1043671, 1043068, 1042466, 1041864, 1041262, 1040661, 1040060, 1039459, 1038859, 1038259, 1037660, 1037060, 1036462, 1035863, 1035265, 1034667, 1034070, 1033472, 1032876, 1032279, 1031683, 1031087, 1030492, 1029897, 1029302, 1028708, 1028114, 1027520, 1026927, 1026334, 1025741, 1025149, 1024557, 1023965, 1023374, 1022783, 1022192, 1021602, 1021012, 1020423, 1019833, 1019244, 1018656, 1018068, 1017480, 1016892, 1016305, 1015718, 1015132, 1014545, 1013959, 1013374, 1012789, 1012204, 1011619, 1011035, 1010451, 1009868, 1009285, 1008702, 1008120, 1007537, 1006956, 1006374, 1005793, 1005212, 1004632, 1004052, 1003472, 1002892, 1002313, 1001734, 1001156, 1000578, 1000000, 999423, 998845, 998269, 997692, 997116, 996540, 995965, 995390, 994815, 994240, 993666, 993092, 992519, 991946, 991373, 990801, 990228, 989657, 989085, 988514, 987943, 987373, 986803, 986233, 985663, 985094, 984525, 983957, 983388, 982821, 982253, 981686, 981119, 980552, 979986, 979420, 978855, 978289, 977725, 977160, 976596, 976032, 975468, 974905, 974342, 973779, 973217, 972655, 972093, 971532, 970971, 970410, 969850, 969290, 968730, 968171, 967612, 967053, 966494, 965936, 965379, 964821, 964264, 963707, 963151, 962594, 962039, 961483, 960928, 960373, 959818, 959264, 958710, 958157, 957603, 957050, 956498, 955945, 955393, 954842, 954290, 953739, 953188, 952638, 952088, 951538, 950989, 950439, 949891, 949342, 948794, 948246, 947698, 947151, 946604, 946058, 945511, 944965, 944420};

//these are the MIDI CC messages that have features the controller will specifically handle
//they include things like CC7 that changes the note velocity maximum for all notes, etc.
//see the handle_cc function for the full list of things handled by the controller.
//Note that all MIDI CC messages are saved, so whatever is using the controller can handle them as needed.
enum MIDI_CC{
	bank_select_msb = 0,
	modulation_wheel_msb = 1,
	breath_controller_msb = 2,
	//CC3 is undefined
	foot_controller_msb = 4,
	portamento_time_msb = 5,
	rpn_data_entry_msb = 6,
	channel_volume_msb = 7,
	balance_msb = 8,
	//CC9 is undefined
	pan_msb = 10,
	expression_controller_msb = 11,
	effect_control_1_msb = 12,
	effect_control_2_msb = 13,
	//CC14-15 are undefined
	//CC16-19 are general purpose controllers 1-4
	//CC20-31 are undefined
	bank_select_lsb = 32,
	modulation_wheel_lsb = 33,
	breath_controller_lsb = 34,
	//CC35 is undefined - Can be LSB for 3
	foot_controller_lsb = 36,
	portamento_time_lsb = 37,
	rpn_data_entry_lsb = 38,
	channel_volume_lsb = 39,
	balance_lsb = 40,
	//CC41 is undefined - Can be LSB for 9
	pan_lsb = 42,
	expression_controller_lsb = 43,
	effect_control_1_lsb = 44,
	effect_control_2_lsb = 45,
	//CC46-47 are undefined - Can be LSB for 14-15
	//CC48-51 are general purpose controllers 1-4 - LSB for 16-19
	//CC52-63 are undefined - Can be LSB for 20-31
	damper_pedal_on_off = 64,
	portamento_on_off = 65,
	sostenuto_on_off = 66,
	soft_pedal_on_off = 67,
	legato_footswitch = 68,
	hold_2 = 69,
	sound_controller_1 = 70, //default: Sound Variation
	sound_controller_2 = 71, //default: Timbre / Harmonic Intensity
	sound_controller_3 = 72, //default: Release Time
	sound_controller_4 = 73, //default: Attach Time
	sound_controller_5 = 74, //default: Brightness
	sound_controller_6 = 75, //default: Decay Time (See MMA RP-021)
	sound_controller_7 = 76, //default: Vibrato Rate (See MMA RP-021)
	sound_controller_8 = 77, //default: Vibrato Depth (See MMA RP-021)
	sound_controller_9 = 78, //default: Vibrato Delay (See MMA RP-021)
	sound_controller_10 = 79, //default: undefined (See MMA RP-021)
	//CC80-83 are general purpose controllers 5-8
	portamento_control = 84,
	//CC85-87 are undefined
	high_resolution_velocity_prefix = 88,
	//CC89-90 are undefined
	effects_1_depth = 91, //Default: Reverb Send Level (See MMA RP-023) Formerly External Effects Depth
	effects_2_depth = 92, //Formerly Tremolo Depth
	effects_3_depth = 93, //Default: Chorus Send Level (See MMA RP-023) Formerly Chorus Depth
	effects_4_depth = 94, //Formerly Celeste [Detune] Depth
	effects_5_depth = 95, //Formerly Phaser Depth
	data_increment = 96, //Data Entry +1 (See MMA RP-018)
	data_decrement = 97, //Data Entry -1 (See MMA RP-018)
	nrpn_lsb = 98,
	nrpn_msb = 99,
	rpn_lsb = 100,
	rpn_msb = 101,
	//CC102-119 are undefined
	all_sound_off = 120,
	reset_all_controllers = 121,
	local_control_on_off = 122,
	all_notes_off = 123,
	omni_mode_off = 124, //also needs to call all notes off
	omni_mode_on = 125, //also needs to call all notes off
	mono_mode_on = 126, //also needs to call all notes off
	poly_mode_on = 127 //also needs to call all notes off
};

//these are MIDI RPN values with a MSB of 0
enum MIDI_RPN_0{
	pitch_bend_sensitivity = 0,
	channel_fine_tuning = 1,
	channel_coarse_tuning = 2,
	tuning_program_change = 3,
	tuning_bank_select = 4,
	modulation_depth_range = 5,
	MPE_configuration_message = 6 //see MPE spec
	//7-127 reserved for future MMA definition
};

//this is how many defined RPN 0 values there are
#define MIDI_NUM_RPN_0 7

//these are MIDI RPN values with a MSB of 0x3D used by three dimensional sound controllers
enum MIDI_RPN_3D{
	azimuth_angle = 0, //See MMA RP-049
	elevation_angle = 1, //See MMA RP-049
	gain = 2, //See MMA RP-049
	distance_ratio = 3, //See MMA RP-049
	maximum_distance = 4, //See MMA RP-049
	gain_at_maximum_distance = 5, //See MMA RP-049
	reference_distance_ratio = 6, //See MMA RP-049
	pan_spread_eagle = 7, //See MMA RP-049
	roll_angle = 8 //See MMA RP-049
	//9-127 reserved for future MMA definition
};

//this is how many defined RPN 3D values there are
#define MIDI_NUM_RPN_3D 7

//thjis is the null value for RPN - if RPN is set to this, it will ignore all data from registers CC6 and CC38
#define MIDI_RPN_NULL 0x7F

//this is a struct for storing MIDI note info:
//it will be kept up to date on every MIDIController::update() call with the currently playing notes.
struct midi_note{
	//this is the MIDI channel from 1-16
	uint8_t channel;
	//this is the MIDI note number from 0 to 127
	uint8_t note;
	//this is the MIDI note velocity from 0-127
	uint8_t velocity;
	//this is the calculated inverted frequency based on the current note manipulations.
	//it is calculated when the note is turned on, and again on updates that effect notes.
	//it will always be up-to-date after any MIDIController::update();
	uint32_t freq;
};

//this MIDIController class is designed to keep track of the current state of all MIDI notes that have been sent since it was created.
//it will log both hardware and USB MIDI information and keep and up-to-date log of all currently playing notes, as well as CC message data
//You should be able to use this to control synths based on the 'current state-of-the-synth' public variables provided by the class.
class MIDIController{
	public:
		//the constructor function for the controller
		MIDIController(void);

		//this needs to be called prior to using any of the MIDIController features.
		void init(void);

		//this needs to be called regularly in your loop to keep the public variables up-to-date.
		void update(void);

		//this resets all the MIDIController variables ot their default values
		void reset_to_default(void);

		//this lets whatever's using the controller check to see if a note was added to the controller
		//it will reset to false every time it is called
		bool note_was_added(void);

		//this lets whatever's using the controller check to see if some note value was changed on the controller
		//it will reset to false every time it is called
		bool note_was_changed(void);

		//this lets whatever's using the controller check to see if some note value was removed from the controller
		//it will reset to false every time it is called
		bool note_was_removed(void);

		//this lets whatever's using the controller check to see if a CC value has changed
		//it will reset to false every time it is called
		bool cc_was_changed(void);

		//this lets whatever's using the controller check to see if the controller program mode has changed
		//it will reset to false every time it is called
		bool program_mode_was_changed(void);

		//this lets whatever's using the controller check to see if a tune request was received
		//it will reset to false every time it is called
		bool tune_request_was_received(void);

		//this lets whatever's using the controller check to see if a system reset request was received
		//it will reset to false every time it is called
		bool system_reset_request_was_received(void);

		//this is an array that tracks that current state of MIDI notes on the controller.
		//it will be regularly updated by the update() function to take into account things like pitch bends and CC messages that effect note values.
		midi_note current_notes[MAX_CONCURRENT_MIDI_NOTES];

		//this is a counter variable that tracks the number of current notes
		uint8_t num_current_notes;

		//this tracks the current pitch bend value for each channel
		//it can be a number from -8191 to 8191
		//it is regularly updated by incoming pitch bend messages
		uint16_t current_pitch_bends[NUM_MIDI_CHANNELS];

		//this tracks the current channel aftertouch values
		//it is regularly updated by incoming Channel pressure messages
		//Note that aftertouch only applies to currently playing notes by default,
		//so this is just usable as a reference and does not control anything to do with new notes
		uint8_t current_channel_aftertouches[NUM_MIDI_CHANNELS];

		//this tracks the current program mode for each channel
		//it is regularly updated by incoming program change messages
		uint8_t current_program_modes[NUM_MIDI_CHANNELS];

		//this tracks the most recently received values for MIDI CC messages
		//for CC messages with defined actions like CC7 (Volume), the MIDIController will automatically adjust note values
		//for others it will be up to the user to write functions that will handle actions depending on the CC message
		//this array is present so that people can check the most recent CC message values at any time and take appropriate action as needed
		uint8_t current_cc_values[NUM_MIDI_CHANNELS][NUM_MIDI_CC_TYPES];

		//this an array that tracks the MSB for RPN values where the RPN MSB (CC 101) is equal to 0
		//the value can be set directly using CC 6 for MSB and CC38 for LSB, 
		//or the values can be incremented/decremented using CC96 and CC97
		uint8_t rpn_0_array[MIDI_NUM_RPN_0];
		
		//this an array that tracks the MSB for RPN values where the RPN MSB (CC 101) is equal to 0x3D
		//the value can be set directly using CC 6 for MSB and CC38 for LSB, 
		//or the values can be incremented/decremented using CC96 and CC97
		uint8_t rpn_3d_array[MIDI_NUM_RPN_3D];
	private:
		//this will handle the hardware MIDI messages and usbMIDI messages 
		void process_MIDI(void);

		//this will take the raw data from the process_hardware_MIDI and process_USB_MIDI functions and call the appropriate handle_* functions
		//it will also allow for debug output of ignored messages id MIDI_DEBUG_IGNORED is true
		void assign_MIDI_handlers(uint8_t type, uint8_t channel, uint8_t data_1, uint8_t data_2);

		//this handles note_on messages received by either hardware or usb MIDI
		void handle_note_on(uint8_t channel, uint8_t note, uint8_t velocity);

		//this handles note off messages received by either hardware or usb MIDI
		void handle_note_off(uint8_t channel, uint8_t note, uint8_t velocity);

		//this handles pitch bend messages received by either hardware or usb MIDI
		void handle_pitch_bend(uint8_t channel, int16_t pitch);

		//this handles channel aftertouch messages received by either hardware or usb MIDI
		void handle_aftertouch_channel(uint8_t channel, uint8_t pressure);

		//this handles poly aftertouch messages received by either hardware or usb MIDI
		void handle_aftertouch_poly(uint8_t channel, uint8_t note, uint8_t velocity);

		//this handles program change messages received by either hardware or usb MIDI
		void handle_program_change(uint8_t channel, uint8_t program);

		//this handles control change messages received by either hardware or usb MIDI
		void handle_control_change(uint8_t channel, uint8_t cc_type, uint8_t cc_value);

		//this handles CC6 and CC38 messages received by either hardware or usb MIDI
		//the results will depend on the most recent RPN messages received on CC100 and CC101
		void handle_rpn_change(uint8_t rpn_msb, uint8_t rpn_lsb, uint8_t cc6_value, uint8_t cc38_value);

		//this handles tune request messages received by either hardware or usb MIDI
		void handle_tune_request(void);

		//this handles system reset messages received by either hardware or usb MIDI
		void handle_system_reset(void);

		//this will add a note to the note array using the current values for pitch bends and applicable CC information
		//If the note is already in the note array, it will be moved to the end of the line, and the remaining notes will be shifted down.
		//if the note array is full and a new note is added, the oldest note will be removed.
		void add_note(uint8_t channel, uint8_t note, uint8_t velocity);

		//this will remove a note from the note array and shift the remaining notes down
		//if the note is not in the array, nothing will change
		void rm_note(uint8_t channel, uint8_t note);

		//this will check to see if a note is in the current_note_array, and return its position if it is.
		//it will return NO_NOTE if it is not.
		int8_t check_note(uint8_t channel, uint8_t note);

		//this will calculate the inverted note frequency in microseconds based on the current pitch bend value for the channel and the note number
		//it will also take into account the max pitch bend values set via MIDI RPN commands if they are not the default value of 2 semitones.
		uint32_t calculate_note_frequency(uint8_t channel, uint8_t note);

		//this will set the pitch bend value and update all current notes as well
		void set_pitch_bend(uint8_t channel, int16_t pitch);

		//this will set the max pitch bend offset, and recalculate the pitch bend for all current notes as well
		void set_max_pitch_bend(uint8_t channel, uint8_t semitones, uint8_t cents);

		//this is a function to print the current_notes array with some formatting for debug pruposes
		void print_current_notes(void);

		//this is a flag that lets something outside the controller know if a new message was received
		//calling note_was_added() will return the current value and reset the value to false
		bool new_note_added;

		//this is a flag that lets something outside the controller know if a new message was received
		//calling note_was_changed() will return the current value and reset the value to false
		bool new_note_changed;

		//this is a flag that lets something outside the controller know if a new message was received
		//calling note_was_removed() will return the current value and reset the value to false
		bool new_note_removed;

		//this is a flag that lets something outside the controller know if a new message was received
		//calling cc_message_was_changed() will return the current value and reset the value to false
		bool new_cc_message;

		//this is a flag that lets something outside the controller know if a new message was received
		//calling program_mode_was_changed() will return the current value and reset the value to false
		bool new_program_mode;

		//this is a flag that lets something outside the controller know if a new message was received
		//calling tune_request_was_received() will return the current value and reset the value to false
		bool new_tune_request;

		//this is a flag that lets something outside the controller know if a new message was received
		//calling system_reset_request_was_received() will return the current value and reset the value to false
		bool new_system_reset_request;
};

#endif

/*

//old stuff from the oMIDItone class that needs to be reintegrated into the new MIDIController class:

//this will set the pitch bend value. This will apply to any notes played on the oMIDItone.
void set_pitch_bend(int16_t pitch_bend_value, uint8_t pitch_bend_channel);

//this sets the max pitch bend for the oMIDItone, used when calculating all pitch bends:
//pitch will bend to a max of semitones + cents above or below the currently_playing_note frequency based on currently_playing_pitch_bend
void set_max_pitch_bend(uint8_t semitones, uint8_t cents);

//this will cancel the current pitch correction testing if anything disruptive happens during the testing to avoid incorrect corrections
void cancel_pitch_correction();

//This adjusts a frequency to a pitch-bent value from the base note.
//note will vary as a proportion of pitch_bend from -8192 to +8192 up to max_pitch_bend_offset cents cents max away from the note frequency.
uint32_t pitch_adjusted_frequency(uint8_t note, int16_t pitch_bend);

*/