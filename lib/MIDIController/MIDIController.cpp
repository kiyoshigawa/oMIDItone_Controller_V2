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

*/