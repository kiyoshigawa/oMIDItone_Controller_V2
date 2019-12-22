/*
This is a library designed to work with the Teensy MIDI libraries to control
a MIDI device (such as a synthesizer) from the Teensy. The goal of the library
is to handle the routine MIDI note messsages so that whatever device you're
using this library on can decide how to play notes or respond to control codes
and output audio or video based on all of the received MIDI messages.

The library is designed to keep track of all incoming MIDI messages and maintain
a 'state-of-the-synth' controller object that has current information based on
the sum total of all MIDI messages received to that point.

The controller will track messages from both a hardware serial MIDI, as well as
from the Teensy emulated USB serial MIDI input. Both are treated the same, and
the sum total of all messages received over both hardware and USB inputs is 
tracked by the controller.

The controller tracks currently playing notes based on incoming note on messages
and updates them based on pitch bend messages, channel aftertouch messages, and
polyphonic aftertouch messages. All notes keep track of the note number,
channel, velocity, and calculated note frequency based on the most current pitch
bend values for that channel. Notes will be removed when note off messages or
note on messages with 0 velocity are received, as well as when some CC messages
that turn off all notes are received (assuming that no user-defined CC handlers
override that functionality).

In addition to tracking notes, the MIDI controller handles and tracks CC 
messages, including most RPN messages. For a full list of what is supported by 
the controller by default, see the MIDI_chart_v2.pdf file included with this 
library.

Most CC messages have no default action associated with them, however all CC 
messages are tracked and the most recent values are stored in arrays on the 
MIDIController object. CC handling functionality can be overridden by user-
created functions to allow maximum flexibility in how to handle the messages 
and effects.

In order for this class to work properly, it is required to repeatedly call the 
update() function on the class object. This will parse any incoming MIDI 
messages in the USB and Hardware MIDI buffers and update all the values. Regular
calls to the update function in an endless loop will keep everything working. It
is not recommended to run this class with any blocking code, as the MIDI buffers
can overflow and message data can be lost if the update() function is not called
often enough.

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
#include <math.h>
#include <MIDI.h>
#include <usb_MIDI.h>

//Below are several options for debug output so you can track incoming MIDI 
//messages and see what your controllers are sending.

//uncomment this to get debug messages about ignored MIDI messages
//#define MIDI_DEBUG_IGNORED

//uncomment this to get debug messages about MIDI Aftertouch
//#define MIDI_DEBUG_AFTERTOUCH

//uncomment this to get debug messages about MIDI Pitch Bends
//#define MIDI_DEBUG_PITCH_BEND

//uncomment this to get debug messages about MIDI program and system messages
//#define MIDI_DEBUG_SYSTEM

//uncomment this to get debug messages about MIDI CC messages from the default handlers
//#define MIDI_DEBUG_CC

//uncomment this to get debug messages about RPN handling from the default handlers
//#define MIDI_DEBUG_RPN

//uncomment this to get printouts of the current note array every time a note is added or removed
//#define MIDI_NOTE_DEBUG

//uncomment this to add more info to the MIDI note debug messages, including velocity and inverted frequency
//#define MIDI_NOTE_DEBUG_VERBOSE

//uncomment this to disable per-channel tuning to save on memory usage of the controller
//uncommenting this flag will save approximately 7.6k of RAM
//#define MIDI_FORCE_GLOBAL_TUNING

//defaults used when creating the hardware MIDI interface
//see teensy MIDI.h library documentation for more info
//if using another hardware MIDI interface, update these.
#define HARDWARE_MIDI_TYPE HardwareSerial
#define HARDWARE_MIDI_INTERFACE Serial1

//this is used in tuning calculations when calculating frequency offsets
#define MIDI_NOTE_A_HZ 440

//This is a non-valid note number for MIDI to designate no note should be played.
#define MIDI_NO_NOTE 128

//This is a non-valid channel number for MIDI to designate no current channel.
#define MIDI_NO_CHANNEL 17

//this is a non-valid array position used to denote that a note was not found in an array
#define MIDI_NOT_IN_ARRAY -1

//This is how many MIDI notes there are. It should always be 128
#define MIDI_NUM_NOTES 128

//This is how many MIDI CC message types there are. It should always be 128
#define MIDI_NUM_CC_TYPES 128

//This is the number of MIDI channels. Should always be 16, unless MIDI changed sometime in the last decade.
#define MIDI_NUM_CHANNELS 16

//this is the maximum value for an 8-bit MIDI message data chunk - used for validity checking:
#define MIDI_MAX_1_BYTE_VALUE 127

//this is the maximum value for a 14-bit MIDI message data chunk - used for validity checking:
#define MIDI_MAX_2_BYTE_VALUE 16383

//this is the most concurrent MIDI notes that the controler will keep track of at any given time:
#define MIDI_MAX_CONCURRENT_NOTES 20

//these are the default values for pitch bend range:
#define MIDI_DEFAULT_PITCH_BEND_SEMITONES 2
#define MIDI_DEFAULT_PITCH_BEND_CENTS 0

//these are the maximum value for pitch bends that can be set by RPN messages:
#define MIDI_MAX_PITCH_BEND_SEMITONES 24
#define MIDI_MAX_PITCH_BEND_CENTS 99

//this is the center value for pitch bending - if the pitch_bend vlaue is set to this, it will play the note without any pitch bending.
//Teensy USB MIDI sends a number from -8192 to 8191, so I am using 0 as the center here.
//The hardware teensy MIDI library does not automatically convert the pitch bend to a signed int, so I do it manually in the assign_MIDI_handlers() function.
#define MIDI_CENTER_PITCH_BEND 0
#define MIDI_OFFSET_PITCH_BEND 8192

//this is the center fine tuning value. The MIDI data is sent as a number from 0-16383, so I convert it to be an integer 
//from -8192 to 8191 in the handle_rpn_fine_tuning() function for use in tuning changes.
#define MIDI_CENTER_FINE_TUNING 0
#define MIDI_OFFSET_FINE_TUNING 8192

//this is the center coarse tuning value. It is only one byte from 0-127, so I convert it to be an
//integer from -64 to 63 in the handle_rpn_coarse_tuning() function for use in tuning changes.
#define MIDI_CENTER_COARSE_TUNING 0
#define MIDI_OFFSET_COARSE_TUNING 64

//this is how much I multiplied the cent frequency ratios by to make things work with integer math
//if I rewrite this for Teensy 4.0, I will probably use the FPU and this will not be necessary anymore.
#define MIDI_CENT_FREQUENCY_RATIO_MULTIPLIER 1000000

//this is the default number of mono voices that the device will operate on in mono modes. 
//It can be changed with MIDI CC126 messages, unless they have been overridden.
#define MIDI_DEFAULT_MONO_VOICES 1

//this is the default value for the omni_off_receive_channel variable. It is the only channel the controller accepts notes for when omni mode is off.
#define MIDI_DEFAULT_RECEIVE_CHANNEL 1

//this is the default state of poly mode. If it is set to on, the controller will accept new notes up to the MIDI_MAX_CONCURRENT_NOTES limit,
//otherwise every new note greater then the num_mono_voices value will push an older note out of the current note array.
#define MIDI_DEFAULT_POLY_STATE true

//this is the default state of the omni mode. If set to true, the controller will accept MIDI input on all channels. 
//If set to false, it will only accept input from the omni_off_receive_channel.
#define MIDI_DEFAULT_OMNI_STATE true

//this is the default state for the local control variable. It's up to the user to decide what to do with this value:
#define MIDI_DEFAULT_LOCAL_CONTROL_STATE true

//this is used when tracking whether RPN or NRPN handling should be active.
#define MIDI_RPN 1
#define MIDI_NRPN 2

//this is the null value for RPN - if RPN is set to this, it will ignore all data from registers CC6 and CC38
#define MIDI_RPN_NULL 0x7F

//these are for determining if a MIDI CC RPN value is valid per the MIDI spec
#define MIDI_RPN_0_MSB 0
#define MIDI_RPN_3D_MSB 0x3D

//these are the MIDI CC message numbers that correspond to increment and decrements of RPN values
#define MIDI_INCREMENT 96
#define MIDI_DECREMENT 97

//this is a datatype of a function for handing MIDI CC messages that can be set 
//by the uder of the class. I use it in an array, so each function will only 
//handle one CC message type, and the only arguments are channel and value.
typedef void (*cc_handler_pointer)(uint8_t channel, uint8_t cc_value);

//these are for custom rpn and nrpm handling functions. since they all take the 
//same number and type of arguments, I only made one new datatype
//for relative functions, data_1 is increment or decrement, and data_2 is amount
//for absolute functions, data_1 is the CC6 value and data_2 is the CC38 value
typedef void (*rpn_handler_pointer)(uint8_t channel, uint8_t rpn_msb, uint8_t rpn_lsb, uint8_t data_1, uint8_t data_2);

//This is an array of MIDI notes and the frequency they correspond to. Turns out it is not needed.
//const double Hz_A440_MIDI_freqs[MIDI_NUM_NOTES] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.5, 25.957, 27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963, 3322.438, 3520, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040, 7458.62, 7902.133, 8372.018, 8869.844, 9397.273, 9956.063, 10548.08, 11175.3, 11839.82, 12543.85};

//This is an array that has converted the MIDI_freqs_Hz array into an array of integers representing us between rising edges for the note frequencies
//This is the default values where A=440Hz. If coarse or fine tuning commands are received, the MIDI_freqs[] array that is part of the class will be updated to new values.
const uint32_t A440_MIDI_freqs[MIDI_NUM_NOTES] = {122309, 115446, 108968, 102848, 97077, 91633, 86490, 81632, 77053, 72727, 68643, 64792, 61154, 57723, 54484, 51427, 48538, 45814, 43243, 40816, 38525, 36363, 34322, 32396, 30578, 28861, 27242, 25712, 24270, 22907, 21622, 20408, 19262, 18181, 17161, 16198, 15289, 14430, 13621, 12856, 12134, 11453, 10810, 10204, 9631, 9090, 8580, 8099, 7644, 7215, 6810, 6428, 6067, 5726, 5405, 5102, 4815, 4545, 4290, 4049, 3822, 3607, 3405, 3214, 3033, 2863, 2702, 2551, 2407, 2272, 2145, 2024, 1911, 1803, 1702, 1607, 1516, 1431, 1351, 1275, 1203, 1136, 1072, 1012, 955, 901, 851, 803, 758, 715, 675, 637, 601, 568, 536, 506, 477, 450, 425, 401, 379, 357, 337, 318, 300, 284, 268, 253, 238, 225, 212, 200, 189, 178, 168, 159, 150, 142, 134, 126, 119, 112, 106, 100, 94, 89, 84, 79};

//this is the ratio that frequencies change from -99 cents to 99 cents, multiplied by MIDI_CENT_FREQUENCY_RATIO_MULTIPLIER, in this case 1,000,000.
const uint32_t cent_frequency_ratios[199] = {1058851, 1058240, 1057629, 1057018, 1056408, 1055798, 1055188, 1054579, 1053970, 1053361, 1052753, 1052145, 1051537, 1050930, 1050323, 1049717, 1049111, 1048505, 1047899, 1047294, 1046689, 1046085, 1045481, 1044877, 1044274, 1043671, 1043068, 1042466, 1041864, 1041262, 1040661, 1040060, 1039459, 1038859, 1038259, 1037660, 1037060, 1036462, 1035863, 1035265, 1034667, 1034070, 1033472, 1032876, 1032279, 1031683, 1031087, 1030492, 1029897, 1029302, 1028708, 1028114, 1027520, 1026927, 1026334, 1025741, 1025149, 1024557, 1023965, 1023374, 1022783, 1022192, 1021602, 1021012, 1020423, 1019833, 1019244, 1018656, 1018068, 1017480, 1016892, 1016305, 1015718, 1015132, 1014545, 1013959, 1013374, 1012789, 1012204, 1011619, 1011035, 1010451, 1009868, 1009285, 1008702, 1008120, 1007537, 1006956, 1006374, 1005793, 1005212, 1004632, 1004052, 1003472, 1002892, 1002313, 1001734, 1001156, 1000578, 1000000, 999423, 998845, 998269, 997692, 997116, 996540, 995965, 995390, 994815, 994240, 993666, 993092, 992519, 991946, 991373, 990801, 990228, 989657, 989085, 988514, 987943, 987373, 986803, 986233, 985663, 985094, 984525, 983957, 983388, 982821, 982253, 981686, 981119, 980552, 979986, 979420, 978855, 978289, 977725, 977160, 976596, 976032, 975468, 974905, 974342, 973779, 973217, 972655, 972093, 971532, 970971, 970410, 969850, 969290, 968730, 968171, 967612, 967053, 966494, 965936, 965379, 964821, 964264, 963707, 963151, 962594, 962039, 961483, 960928, 960373, 959818, 959264, 958710, 958157, 957603, 957050, 956498, 955945, 955393, 954842, 954290, 953739, 953188, 952638, 952088, 951538, 950989, 950439, 949891, 949342, 948794, 948246, 947698, 947151, 946604, 946058, 945511, 944965, 944420};

/*
These are MIDI CC values and their corresponding defined functions per the 
official MIDI specs. Some of them perform default functions on the 
MIDIController class, while most do not. CC values 6, 38, 96-101, and 120-127 
all have default handlers assigned per the MIDI specs. It is NOT recommended to
assign custom handlers to these values unless you want to go against the default
MIDI specifications for their behavior.

Assigning custom handlers to any of the above CC values will cause other 
functionality of the MIDIController class to break. This includes RPN settings
such as changing the pitch bend offsets, tuning functions, and 3D synth 
parameters. Modifying CC120-127 will break useful features like omni/poly/mono
mode support, as well as the all notes off, all sounds off, reset all 
controllers, and local control settings. 

Only assign these handlers if you know what you're doing and dislike the MIDI
official specification defaults.
*/
enum MIDI_CC{
	bank_select_msb = 0,
	modulation_wheel_msb = 1,
	breath_controller_msb = 2,
	undefined_3 = 3,
	foot_controller_msb = 4,
	portamento_time_msb = 5,
	rpn_data_entry_msb = 6,
	channel_volume_msb = 7,
	balance_msb = 8,
	undefined_9 = 9,
	pan_msb = 10,
	expression_controller_msb = 11,
	effect_control_1_msb = 12,
	effect_control_2_msb = 13,
	undefined_14 = 14,
	undefined_15 = 15,
	gp1_msb = 16,
	gp2_msb = 17,
	gp3_msb = 18,
	gp4_msb = 19,
	undefined_20 = 20,
	undefined_21 = 21,
	undefined_22 = 22,
	undefined_23 = 23,
	undefined_24 = 24,
	undefined_25 = 25,
	undefined_26 = 26,
	undefined_27 = 27,
	undefined_28 = 28,
	undefined_29 = 29,
	undefined_30 = 30,
	undefined_31 = 31,
	bank_select_lsb = 32,
	modulation_wheel_lsb = 33,
	breath_controller_lsb = 34,
	undefined_35 = 35, //can be LSB for 3
	foot_controller_lsb = 36,
	portamento_time_lsb = 37,
	rpn_data_entry_lsb = 38,
	channel_volume_lsb = 39,
	balance_lsb = 40,
	undefined_41 = 41, //can be LSB for 9
	pan_lsb = 42,
	expression_controller_lsb = 43,
	effect_control_1_lsb = 44,
	effect_control_2_lsb = 45,
	undefined_46 = 46, //can be LSB for 14
	undefined_47 = 47, //can be LSB for 15
	gp1_lsb = 48,
	gp2_lsb = 49,
	gp3_lsb = 50,
	gp4_lsb = 51,
	undefined_52 = 52, //can be lsb for 20
	undefined_53 = 53, //can be lsb for 21
	undefined_54 = 54, //can be lsb for 22
	undefined_55 = 55, //can be lsb for 23
	undefined_56 = 56, //can be lsb for 24
	undefined_57 = 57, //can be lsb for 25
	undefined_58 = 58, //can be lsb for 26
	undefined_59 = 59, //can be lsb for 27
	undefined_60 = 60, //can be lsb for 28
	undefined_61 = 61, //can be lsb for 29
	undefined_62 = 62, //can be lsb for 30
	undefined_63 = 63, //can be lsb for 31
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
	gp5 = 80,
	gp6 = 81,
	gp7 = 82,
	gp8 = 83,
	portamento_control = 84,
	undefined_85 = 85,
	undefined_86 = 86,
	undefined_87 = 87,
	high_resolution_velocity_prefix = 88,
	undefined_89 = 89,
	undefined_90 = 90,
	effects_1_depth = 91, //Default: Reverb Send Level (See MMA RP-023) Formerly External Effects Depth
	effects_2_depth = 92, //Formerly Tremolo Depth
	effects_3_depth = 93, //Default: Chorus Send Level (See MMA RP-023) Formerly Chorus Depth
	effects_4_depth = 94, //Formerly Celeste [Detune] Depth
	effects_5_depth = 95, //Formerly Phaser Depth
	data_increment = 96, //Data Entry +1 (See MMA RP-018) //See Note 1
	data_decrement = 97, //Data Entry -1 (See MMA RP-018) //See Note 1
	nrpn_lsb = 98,
	nrpn_msb = 99,
	rpn_lsb = 100,
	rpn_msb = 101,
	undefined_102 = 102,
	undefined_103 = 103,
	undefined_104 = 104,
	undefined_105 = 105,
	undefined_106 = 106,
	undefined_107 = 107,
	undefined_108 = 108,
	undefined_109 = 109,
	undefined_110 = 110,
	undefined_111 = 111,
	undefined_112 = 112,
	undefined_113 = 113,
	undefined_114 = 114,
	undefined_115 = 115,
	undefined_116 = 116,
	undefined_117 = 117,
	undefined_118 = 118,
	undefined_119 = 119,
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
//using this to keep array sizes smaller
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
//using this to keep array sizes smaller
#define MIDI_NUM_RPN_3D 7

//this is a struct for storing MIDI note info:
//it will be kept up to date on every MIDIController::update() call with the currently playing notes.
struct MIDI_note{
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

		/*
		This will assign user functions to be called when specific MIDI CC 
		messages are received. This will work for any MIDI CC message, but there 
		are consequences... If using this for any one of MIDI CC 6, 38, 96, 97, 
		98, 99, 100, or 101, all RPN and NRPN features will be disabled upon the
		assignment of any one of these CC values. If using for MIDI CC 120-127, 
		the functionality of the MIDI channel messages will break. This includes
		all sound off, all notes off, local control, reset all controllers, 
		omni, poly, and mono modes.
		It is HIGHLY recommended that user functions only be used for the MIDI 
		CC values not listed above to ensure compatibility with standard MIDI
		features using this controller unless you know what you're breaking.
		*/
		void assign_MIDI_cc_handler(uint8_t cc_number, cc_handler_pointer fptr);

		//This allows for handing of RPN messages by user functions instead of 
		//by the MIDI standard specified actions. Overriding these will disable
		//the default actions taken based on the MIDI specs. Do so your own risk
		void assign_MIDI_rpn_absolute_handler(rpn_handler_pointer fptr);

		//This allows for handing of RPN messages by user functions instead of 
		//by the MIDI standard specified actions. Overriding these will disable
		//the default actions taken based on the MIDI specs. Do so your own risk
		void assign_MIDI_rpn_relative_handler(rpn_handler_pointer fptr);

		//this allows for handing of NRPN messages by user functions. NRPNs are
		//entirely user defined, so use these for whatever you want.
		void assign_MIDI_nrpn_absolute_handler(rpn_handler_pointer fptr);

		//this allows for handing of NRPN messages by user functions. NRPNs are
		//entirely user defined, so use these for whatever you want.
		void assign_MIDI_nrpn_relative_handler(rpn_handler_pointer fptr);

		//this sets the receive channel for when omni mode is off. It accepts a MIDI channel value from 0-15.
		void set_omni_off_receive_channel(uint8_t channel);

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

		//this returns the state of the local control variable so that action can be taken by the controller user
		bool is_local_control_enabled(void);

		//this will check to see if a note is in the current_note_array, and return its position if it is.
		//it will return MIDI_NO_NOTE if it is not.
		int8_t check_note(uint8_t channel, uint8_t note);

		//this is an array that tracks that current state of MIDI notes on the controller.
		//it will be regularly updated by the update() function to take into account things like pitch bends and CC messages that effect note values.
		MIDI_note current_notes[MIDI_MAX_CONCURRENT_NOTES];

		//this is a counter variable that tracks the number of current notes
		uint8_t num_current_notes;

		//this tracks the current pitch bend value for each channel
		//it can be a number from -8191 to 8191
		//it is regularly updated by incoming pitch bend messages
		int16_t current_pitch_bends[MIDI_NUM_CHANNELS];

		//this tracks the current channel aftertouch values
		//it is regularly updated by incoming Channel pressure messages
		//Note that aftertouch only applies to currently playing notes by default,
		//so this is just usable as a reference and does not control anything to do with new notes
		uint8_t current_channel_aftertouches[MIDI_NUM_CHANNELS];

		//this tracks the current program mode for each channel
		//it is regularly updated by incoming program change messages
		uint8_t current_program_modes[MIDI_NUM_CHANNELS];

		//this tracks the most recently received values for MIDI CC messages
		//for CC messages with defined actions like CC7 (Volume), the MIDIController will automatically adjust note values
		//for others it will be up to the user to write functions that will handle actions depending on the CC message
		//this array is present so that people can check the most recent CC message values at any time and take appropriate action as needed
		uint8_t current_cc_values[MIDI_NUM_CHANNELS][MIDI_NUM_CC_TYPES];

		//this an array that tracks the MSB for RPN values where the RPN MSB (CC 101) is equal to 0
		//note that the values stores are both msb and lsb together as a single integer.
		//to get the msb separate from the lsb, you need to >> 7, and to get the lsb separate from
		//the msb you need to bitmask with 0x7F
		uint16_t rpn_0_values[MIDI_NUM_CHANNELS][MIDI_NUM_RPN_0];
		
		//this an array that tracks the MSB for RPN values where the RPN MSB (CC 101) is equal to 0x3D
		//note that the values stores are both msb and lsb together as a single integer.
		//to get the msb separate from the lsb, you need to >> 7, and to get the lsb separate from
		//the msb you need to bitmask with 0x7F
		uint16_t rpn_3d_values[MIDI_NUM_CHANNELS][MIDI_NUM_RPN_3D];

		//this stores the per-channel max pitch bend offset allowed by the controller
		//the value is stored in cents offset off the original note number,
		//i.e. an offset of negative 235 is 2 semitones and 35 cents lower than the original note.
		uint16_t max_pitch_bend_offsets[MIDI_NUM_CHANNELS];

		//this is an array of the most recently received fine tuning offsets received from RPN commands
		//it defaults to MIDI_CENTER_FINE_TUNING on init.
		int16_t fine_tuning_offsets[MIDI_NUM_CHANNELS];

		//this is an array of the most recently received coarse tuning offsets received from RPN commands
		//it defaults to MIDI_CENTER_COARSE_TUNING on init.
		int8_t coarse_tuning_offsets[MIDI_NUM_CHANNELS];

		//This is an array that has converted the MIDI_freqs_Hz array into an array of integers representing us between rising edges for the note frequencies
		//These can be modified upon receipt of tuning requests from MIDI RPn commands. 
		//They will always be the current modified note frequency reference for that channel during operation.
		
		//Currently supporting per-channel tuning on the Teensy 3.1 with this code base.
		//If you are running out of RAM, I've got a commented out line above everywhere this variable is used 
		//that should be a drop-in replacement disabling per-channel tuning and going to a single global tuning
		#ifdef MIDI_FORCE_GLOBAL_TUNING
			uint32_t MIDI_freqs[MIDI_NUM_NOTES];
		#else
			uint32_t MIDI_freqs[MIDI_NUM_CHANNELS][MIDI_NUM_NOTES];
		#endif
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

		//this handled rpm modes that receive explicit data set points (i.e. CC6 or CC38)
		//if any of CC6, 38, 96, 97, 98, 99, 100, or 101 have custom handlers, it will not be called.
		void handle_rpn_change_absolute(uint8_t channel, uint8_t rpn_msb, uint8_t rpn_lsb, uint8_t cc6_value, uint8_t cc38_value);

		//this handled rpm modes that receive relative data set points (i.e. CC96 or CC97)
		//if any of CC6, 38, 96, 97, 98, 99, 100, or 101 have custom handlers, it will not be called.
		void handle_rpn_change_relative(uint8_t channel, uint8_t rpn_msb, uint8_t rpn_lsb, uint8_t increment_or_decrement, uint8_t amount);

		//this handled nrpm modes that receive explicit data set points (i.e. CC6 or CC38)
		//if any of CC6, 38, 96, 97, 98, 99, 100, or 101 have custom handlers, it will not be called.
		void handle_nrpn_change_absolute(uint8_t channel, uint8_t nrpn_msb, uint8_t nrpn_lsb, uint8_t cc6_value, uint8_t cc38_value);

		//this handled nrpm modes that receive relative data set points (i.e. CC96 or CC97)
		//if any of CC6, 38, 96, 97, 98, 99, 100, or 101 have custom handlers, it will not be called.
		void handle_nrpn_change_relative(uint8_t channel, uint8_t nrpn_msb, uint8_t nrpn_lsb, uint8_t increment_or_decrement, uint8_t amount);

		//this handles RPN pitch bend sensitivity changes
		//function takes MSB and LSB as a single 14-bit value: new_value
		void handle_rpn_pitch_bend_sensitivity(uint8_t channel, uint16_t new_value);

		//this handles RPN fine tuning changes
		//function takes MSB and LSB as a single 14-bit value: new_value
		void handle_rpn_fine_tuning(uint8_t channel, uint16_t new_value);

		//this handles RPN coarse tuning changes
		//function takes MSB and LSB as a single 14-bit value: new_value
		void handle_rpn_coarse_tuning(uint8_t channel, uint16_t new_value);

		//this handles RPN tuning program change changes
		//function takes MSB and LSB as a single 14-bit value: new_value
		void handle_rpn_tuning_program_change(uint8_t channel, uint16_t new_value);

		//this handles RPN tuning bank select changes
		//function takes MSB and LSB as a single 14-bit value: new_value
		void handle_rpn_tuning_bank_select(uint8_t channel, uint16_t new_value);

		//this handles RPN modulation depth changes
		//function takes MSB and LSB as a single 14-bit value: new_value
		void handle_rpn_modulation_depth_range(uint8_t channel, uint16_t new_value);

		//this handles RPN MPE configuration message changes
		//function takes MSB and LSB as a single 14-bit value: new_value
		void handle_rpn_MPE_configuration_message(uint8_t channel, uint16_t new_value);

		//this handles tune request messages received by either hardware or usb MIDI
		void handle_tune_request(void);

		//this handles system reset messages received by either hardware or usb MIDI
		void handle_system_reset(void);
		
		//this resets all the MIDIController variables ot their default values
		void reset_to_default(void);

		//this will add a note to the note array using the current values for pitch bends and applicable CC information
		//If the note is already in the note array, it will be moved to the end of the line, and the remaining notes will be shifted down.
		//if the note array is full and a new note is added, the oldest note will be removed.
		void add_note(uint8_t channel, uint8_t note, uint8_t velocity);

		//this will remove a note from the note array and shift the remaining notes down
		//if the note is not in the array, nothing will change
		void rm_note(uint8_t channel, uint8_t note);

		//this will calculate the inverted note frequency in microseconds based on the current pitch bend value for the channel and the note number
		//it will also take into account the max pitch bend values set via MIDI RPN commands if they are not the default value of 2 semitones.
		uint32_t calculate_note_frequency(uint8_t channel, uint8_t note);

		//this will recalculate all the values in the MIDI_freqs array for the channel
		//the offset value is a number from -6499 to 6499 in cents as an offset of 
		//the base frequency of A=440Hz that is used to recalculate all the values
		void recalculate_MIDI_freqs(uint8_t channel, int16_t offset_value);

		//this will set the pitch bend value and update all current notes as well
		void set_pitch_bend(uint8_t channel, int16_t pitch);

		//this will set the max pitch bend offset, and recalculate the pitch bend for all current notes as well
		void set_max_pitch_bend(uint8_t channel, uint8_t semitones, uint8_t cents);

		//this is used to make sure 14-bit MIDI values made from multiple messages 
		//are between 0 and MIDI_MAX_2_BYTE_VALUE. Returns a number within that range.
		//if the value submitted is too big, it will return MIDI_MAX_2_BYTE_VALUE,
		//if the value is negative, it will return 0.
		uint16_t clean_2_byte_MIDI_value(int32_t value);

		//this is used to make sure 7-bit MIDI values are between 0 and 
		//MIDI_MAX_1_BYTE_VALUE. Returns a number within that range.
		//if the value submitted is too big, it will return MIDI_MAX_1_BYTE_VALUE,
		//if the value is negative, it will return 0.
		uint8_t clean_1_byte_MIDI_value(int16_t value);

		//this function will set the max pitch bend offset value in cents,
		//when given an offset value in semitones and an offset value in cents.
		//the full range of pitch bending will be from -max_pitch_bend_offset to
		//max_pitch_bend_offset in cents.
		//This is typically called by the RPN function handler, so it will stop
		//working automatically if any RPN CC messages are using overridden handlers.
		void set_max_pitch_bend(uint8_t channel, int8_t semitones, int8_t cents);

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

		//this is a flag to let the controler know if local control is on or off
		//if local control is off, the controller will ignore the hardware MIDI 
		//and will only respond to USB MIDI messages.
		bool local_control_is_enabled;

		/*
		this tracks poly mode for use in parsing incoming messages. When true
		the controller will keep adding notes to the note array up to the
		MIDI_MAX_CONCURRENT_NOTES value.
		When false the controller will only add notes to the controller up to 
		the default value of MIDI_DEFAULT_MONO_VOICES value or to the value that 
		was set  by the MIDI CC126 command, assuming it has not been overridden.
		*/
		bool poly_mode_is_enabled;

		//this is the number of notes in the note array allowed when mono mode 
		//is enabled. It is set to MIDI_DEFAULT_MONO_VOICES on init or reset,
		//but it can be changed by MIDI CC126 commands as well.
		uint8_t num_mono_voices;

		//this tracks the omni mode. When true, the controller accepts messages 
		//on all channels. When false it will only accept input from the 
		//current mono_mode_channel
		bool omni_mode_is_enabled;

		//this is the only channel the receiver will accept MIDI input from when
		//omni mode is disabled. It defaults to MIDI_DEFAULT_RECEIVE_CHANNEL and
		//can be changed using the set_omni_off_receive_channel public function.
		int omni_off_receive_channel;

		//this tracks whether an RPN or NRPN message was most recently sent so
		//that the CC handler function can call the appropriate handlers when
		//CC6, CC38, CC96, and CC97 are received.
		int last_rpn_nrpn_type;

};
#endif