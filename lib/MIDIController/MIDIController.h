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

//Some definitions about MIDI to make the code easier to read:

//This is a non-valid note number for MIDI to designate no note should be played.
#define NO_NOTE 128

//This is a non-valid channel number for MIDI to designate no current channel.
#define NO_CHANNEL 17

//This is the frequency of the note A - typically 440 Hz.
#define NOTE_A 440

//This is how many MIDI notes there are. It should always be 128
#define NUM_MIDI_NOTES 128

//This is the number of MIDI channels. Should always be 16, unless MIDI changed sometime in the last decade.
#define NUM_MIDI_CHANNELS 16

//these are the default values for pitch bend range:
#define DEFAULT_MIDI_PITCH_BEND_SEMITONES 2
#define DEFAULT_MIDI_PITCH_BEND_CENTS 0
#define MAX_PITCH_BEND_SEMITONES 24
#define MAX_PITCH_BEND_CENTS 99

//this is the center value for pitch bending - if the pitch_bend vlaue is set to this, it will play the note without any pitch bending.
//Teensy MIDI sends a number form -8192 to 8191, so I am using 0 as the center here. Typical MIDI center is 8192, adjust as needed.
#define CENTER_PITCH_BEND 0

//this is how much I multiplied the cent frequency ratios by to make things work with integer math
#define CENT_FREQUENCY_RATIO_MULTIPLIER 1000000

//This is an array of MIDI notes and the frequency they correspond to. Turns out it is not needed.
//const float midi_Hz_freqs[NUM_MIDI_NOTES] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.5, 25.957, 27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963, 3322.438, 3520, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040, 7458.62, 7902.133, 8372.018, 8869.844, 9397.273, 9956.063, 10548.08, 11175.3, 11839.82, 12543.85};

//This is an array that has converted the midi_freqs_Hz array into an array of integers representing us between rising edges for the note frequencies
const uint32_t midi_freqs[NUM_MIDI_NOTES] = {122309, 115446, 108968, 102848, 97077, 91633, 86490, 81632, 77053, 72727, 68643, 64792, 61154, 57723, 54484, 51427, 48538, 45814, 43243, 40816, 38525, 36363, 34322, 32396, 30578, 28861, 27242, 25712, 24270, 22907, 21622, 20408, 19262, 18181, 17161, 16198, 15289, 14430, 13621, 12856, 12134, 11453, 10810, 10204, 9631, 9090, 8580, 8099, 7644, 7215, 6810, 6428, 6067, 5726, 5405, 5102, 4815, 4545, 4290, 4049, 3822, 3607, 3405, 3214, 3033, 2863, 2702, 2551, 2407, 2272, 2145, 2024, 1911, 1803, 1702, 1607, 1516, 1431, 1351, 1275, 1203, 1136, 1072, 1012, 955, 901, 851, 803, 758, 715, 675, 637, 601, 568, 536, 506, 477, 450, 425, 401, 379, 357, 337, 318, 300, 284, 268, 253, 238, 225, 212, 200, 189, 178, 168, 159, 150, 142, 134, 126, 119, 112, 106, 100, 94, 89, 84, 79};

//this is the ratio that frequencies change from -99 cents to 99 cents, multiplied by CENT_FREQUENCY_RATIO_MULTIPLIER, in this case 1,000,000.
const uint32_t cent_frequency_ratios[199] = {1058851, 1058240, 1057629, 1057018, 1056408, 1055798, 1055188, 1054579, 1053970, 1053361, 1052753, 1052145, 1051537, 1050930, 1050323, 1049717, 1049111, 1048505, 1047899, 1047294, 1046689, 1046085, 1045481, 1044877, 1044274, 1043671, 1043068, 1042466, 1041864, 1041262, 1040661, 1040060, 1039459, 1038859, 1038259, 1037660, 1037060, 1036462, 1035863, 1035265, 1034667, 1034070, 1033472, 1032876, 1032279, 1031683, 1031087, 1030492, 1029897, 1029302, 1028708, 1028114, 1027520, 1026927, 1026334, 1025741, 1025149, 1024557, 1023965, 1023374, 1022783, 1022192, 1021602, 1021012, 1020423, 1019833, 1019244, 1018656, 1018068, 1017480, 1016892, 1016305, 1015718, 1015132, 1014545, 1013959, 1013374, 1012789, 1012204, 1011619, 1011035, 1010451, 1009868, 1009285, 1008702, 1008120, 1007537, 1006956, 1006374, 1005793, 1005212, 1004632, 1004052, 1003472, 1002892, 1002313, 1001734, 1001156, 1000578, 1000000, 999423, 998845, 998269, 997692, 997116, 996540, 995965, 995390, 994815, 994240, 993666, 993092, 992519, 991946, 991373, 990801, 990228, 989657, 989085, 988514, 987943, 987373, 986803, 986233, 985663, 985094, 984525, 983957, 983388, 982821, 982253, 981686, 981119, 980552, 979986, 979420, 978855, 978289, 977725, 977160, 976596, 976032, 975468, 974905, 974342, 973779, 973217, 972655, 972093, 971532, 970971, 970410, 969850, 969290, 968730, 968171, 967612, 967053, 966494, 965936, 965379, 964821, 964264, 963707, 963151, 962594, 962039, 961483, 960928, 960373, 959818, 959264, 958710, 958157, 957603, 957050, 956498, 955945, 955393, 954842, 954290, 953739, 953188, 952638, 952088, 951538, 950989, 950439, 949891, 949342, 948794, 948246, 947698, 947151, 946604, 946058, 945511, 944965, 944420};


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