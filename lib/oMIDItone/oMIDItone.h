/*
This is the oMIDItone library, to send commands to a pair of MCP4151 digital
pots to change the tone on an otamatone.

I have set it up so there is one 100k pot, and 1 50k pot. The first 512 steps 
oscilate the 50k pot between 0 and 1 so that I maintain the higher resolution,
and the final 256 steps are directly modifying the 50k pot. Make sure the
first pot pin is the 100k and the second is the 50k.

It also has a pin for turning the sound on and off, and a pin for
analog readings to correct the frequency being played.

The oMIDItone class objects will be able to be set to output a specific 
frequency that is within the range measured during their initialization, and 
will try to maintain that frequency as long as pitch correction is enabled and 
they have not been requested to turn off the note.

The class object will also keep track of the lighting animation state (even 
though the animation triggers will be controlled externally), as well 
as control the servos that open and close the mouth whenever the sound output
is set to be on or off.

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

#ifndef OMIDITONE_H
#define OMIDITONE_H

#include <Arduino.h>

//We will need the SPI library to communicate with the MCP4151 chips.
#include <SPI.h>

//We will also use the fancy faster ADC library from pedvide for Teensy:
#include <ADC.h>

//This is for the PCA9685 Servo controller.
#include <i2c_t3.h>
#include <Adafruit_PWMServoDriver.h>

//this is for the LED lighting so the animation for the head can be stored on the head.
#include <lighting_control.h>

//This prints out general debug messages for the head.
//this includes startup test messages and messages that let you know when notes are measured out of range
#define OMIDITONE_DEBUG

//comment this out to disable pitch debug messages
//this will turn on or off output messages about the frequency measurements in the measure_freq() function:
//#define PITCH_DEBUG

//comment this out to disable verbose frequency adjustment messages for every change in frequency
//#define PITCH_VERBOSE_DEBUG

//comment this out to disable startup test in-depth frequency printouts:
//#define STARTUP_PITCH_MEASUREMENT_DEBUG

//this controls default state of frequency correction
#define FREQ_CORRECTION_DEFAULT_ENABLE_STATE true

//this controls default state of servos
#define SERVO_DEFAULT_ENABLE_STATE true

//number of oMIDItones on this controller - Update as needed.
#define NUM_OMIDITONES 6

//this is a non-valid frequency value to denote that no sound should be output.
#define NO_FREQ 0

//this is how many resistance steps can be used with the digital pots. The current hardware has 2 digital pots with 256 steps each, for a total of 512.
#define NUM_RESISTANCE_STEPS 765

//This is the analog read threshold for a rising edge to count the frequency.
#define RISING_EDGE_THRESHOLD 50

//this is the % difference that a note can be off to trigger correction, as a number from 0-100
#define ALLOWABLE_NOTE_ERROR 1

//This is the % off that a frequency reading can be before it's determined to be invalid and thrown out, as a number from 0-100:
#define ALLOWABLE_FREQ_READING_VARIANCE 50

//this is a multiplier number to check for unreasonably large frequency measurements during the initial startup test.
#define UNREASONABLY_LARGE_MULTIPLIER 2

//this is to make sure that the rising edge isn't measured too often (in us):
#define MIN_TIME_BETWEEN_RISING_EDGE_MEASUREMENTS 1

//Time to wait between receiving a note and starting to play that note (in ms).
#define NOTE_WAIT_TIME 10

//this is to make sure frequency corrections are not too frequent (in us):
#define TIME_BETWEEN_FREQ_CORRECTIONS 20

//this is a jitter value to randomize the resistance in an attempt to counter the frequency variation around a specific resistance value. it is measured in resistance steps
#define JITTER 1

//This is the number of rising edges to read before computing a new current average frequency.
#define NUM_FREQ_READINGS 5

//This forces the init to run for INIT_MULTIPLIER*NUM_FREQ_READINGS of rising edges before taking the frequency reading on init.
//Hopefully this will reduce or remove the need for the STABILIZATION_TIME startup testing.
#define INIT_MULTIPLIER 17

//This is how long to wait for initial frequency readings on init before declaring failure and marking the object as unavailable in ms.
//if it sounds like things should be working, but it keeps timing out, you may need to increase this value.
#define TIME_TO_WAIT_FOR_INIT 2000

//THis is how long to play an initial note before the startup_test sets midi_freqs. in ms
#define TIME_TO_WAIT_FOR_STARTUP_TEST_SOUND 100

//This is how long to wait for a single reading to timeout. Increase if low notes are reading highter than they should. in ms
#define NOTE_TIMEOUT 2000

//If a frequency is measured at this or higher, it will stop incrementing resistances, as it's too high to measure.
#define MAX_FREQ 300

//This is the address of the Adafruit PCA9685 servo controller used by all the servos on the project.
//If yours has a different I2C adderess bit set, you will need to change it here.
//Note that this code assumed a maximum of 16 servos, with only one controller that will work for all oMIDItone heads.
#define PCA9685_ADDRESS 0x41

//This is for setting the base frequency of the PCA9685. By default it is 240Hz, which results in a ~1us per step pulse length
//Don't mess with this unless you've got a very good reason for needing something other than ~1us per step.
//All the servo max and min pulse length values assume this is set to 240Hz resulting in ~1us pulse steps.
#define PCA9685_FREQ 240

//This is the OE pin for the controller board. One pin controls all the servos, so it's defined at the top of the class instead of per head.
#define PCA9685_OE_PIN 20

//This is the number of LEDs on the WS2812 strip used for the head, including in front, back-top, and back-bottom:
#define NUM_LEDS_PER_HEAD 18

//This is how often servo updates can be sent in us. (About 60Hz)
#define TIME_BETWEEN_SERVO_MOVEMENTS 16

class oMIDItone {
	public:
		//constructor function
		oMIDItone(uint16_t signal_enable_optoisolator, uint16_t speaker_disable_optoisolator, uint16_t cs1, uint16_t cs2, uint16_t feedback, uint16_t servo_l_channel, uint16_t servo_r_channel, uint16_t servo_l_min, uint16_t servo_l_max, uint16_t servo_r_min, uint16_t servo_r_max, uint16_t led_head_array[NUM_LEDS_PER_HEAD], Animation * head_animation);

		//this will init the pin modes and set up Serial if it's not already running.
		void init(void);

		//This should be called during the loop, and it will update the note frequencies and play notes as needed.
		void update(void);

		//This will tell the oMIDItone to play at a frequency. The frequency will continue to play until changed or until sound is set to off.
		//If the note is out of the oMIDItone range, it will not play anything and return false
		//if the note can be played, it will begin playing immediately and return true
		bool play_freq(uint32_t freq);

		//this is basically the same as play_freq, but it won't have a NOTE_WAIT_TIME length pause before it begins playing the note.
		//useful for handing pitch bends that occur after a note has begun playing
		bool update_freq(uint32_t freq);

		//This will set the oMIDItone to stop playing any sound.
		void sound_off(void);

		//this allows manual setting of servos when they are disabled by the above function:
		//position is a value between 0 and 127, 0 being closed, 127 being open.
		void set_servos(uint16_t position);

		//This will return true if init was successful AND there is no current frequency playing.
		bool is_ready(void);

		//This returns the inverted frequency in microseconds that is currently playing or NO_FREQ if no note is currently playing.
		uint16_t currently_playing_freq(void);

		//this will let the head know that the current frequency reading is likely compromised due to external timing factors
		//once called, the pitch correction will reset and begin collecting readings to be adjusted from scratch
		void cancel_pitch_correction(void);

		//these enable and disable frequency correction:
		void enable_pitch_correction(void);
		void disable_pitch_correction(void);

		//these enable and disable servos:
		void enable_servos(void);
		void disable_servos(void);

		//this stores the lighting animation info for the head:
		Animation * animation;

	private:
		//This will play from 0 resistance value to 512 resistance value and note which resistances
		//correspond to which notes in the note matrix:
		//Returns true if init was successful and false if it was not.
		bool startup_test(void);

		//this will change the resistance value and set the current_desired_freq for pitch correction to the frequency in the argument.
		//do not call without making sure the frequency is playable first
		void set_freq(uint32_t freq);

		//this takes the frequency averaging code and puts it into a function to clean up the update function:
		void measure_freq(void);

		//This is a function that will change the current_resistance to a different value if it is too far off from the current_frequency.
		void adjust_freq(void);

		//This will check if an inverted frequency (in us) can be played by an initialized oMIDItone object.
		bool can_play_freq(uint32_t freq);

		//This function will open and close the mouth based on the current note valocity
		void servo_update(void);

		//This will constantly read the analog input and return true when it detects a rising edge signal.
		//It also updates the current and last rising edge time values.
		bool is_rising_edge(void);

		//a simple averaging function:
		uint32_t average(unsigned long * array, uint16_t num_elements);

		//this function will find a resistance value that was measured as being very near the desired frequency.
		uint16_t freq_to_resistance(uint16_t freq);

		//this introduces jittered resistance settings, and should be called every loop to keep the jitter working:
		void set_jitter_resistance(uint16_t resistance, uint16_t jitter);

		//this will take a uint16_t number and set the total resistance value to between 0 and 767 on the board.
		void set_resistance(uint16_t resistance);

		//this will set the CS_pin digital pot's wiper to a value based on byte 1 and byte 2
		void set_pot(uint16_t CS_pin, uint16_t command_byte);

		//This will be set to true if the startup_test was successful:
		bool had_successful_init;

		//this is a variable that controls whether or not frequency correction is enabled:
		bool pitch_correction_is_enabled;

		//this is a variable that controls whether or not servos are enabled
		bool servo_is_enabled;

		//this will be set during the startup test to the lowest inverted frequency registered.
		uint32_t min_freq;

		//this will be set during the startup test to the highest inverted frequency registered.
		uint32_t max_freq;

		//this is an array of the most recent measured rising edge average times in us that correspond to a resistance
		uint32_t measured_freqs[NUM_RESISTANCE_STEPS];

		//This is an array of the last NUM_FREQ_READINGS frequency readings for averaging purposes.
		uint32_t recent_freqs[NUM_FREQ_READINGS];

		//this is an index that will count up to NUM_FREQ_READINGS and reset to 0, triggering a frequency measurement average.
		uint16_t freq_reading_index;

		//This is a variable for storing the most recent frequency reading based on the average of the last NUM_FREQ_READINGS readings
		uint32_t current_freq;

		//this is a variable that stores the current desired frequency set by the play_freq() or change_freq() functions
		//it cuts down on calculating it every time, since pitch bend uses floating point math, which is much slower than the rest of the code
		uint32_t current_desired_freq;

		//this is a variable for globally storing the most recent analog reading
		uint16_t last_analog_read;

		//variable for saving the current resistance value of the digital pots.
		uint16_t current_resistance;

		//this is used to cancel a pitch correction if an event occurs that would disturb the timing
		bool pitch_correction_has_been_compromised;

		//pin number tracking:
		uint16_t signal_enable_optoisolator_pin;
		uint16_t speaker_disable_optoisolator_pin;
		uint16_t cs1_pin;
		uint16_t cs2_pin;
		uint16_t analog_feedback_pin;

		//set up an instanced ADC object for use in the code.
		ADC *adc = new ADC();

		//Servo channels - these are the channel for the left and right servo for this head on the servo controller
		uint16_t l_channel;
		uint16_t r_channel;

		//Servo Position values - these set the max and min extent of servo pulse width in us
		uint16_t l_min;
		uint16_t l_max;
		uint16_t r_min;
		uint16_t r_max;

		//this is the servo controller object that will run all the servos:
		Adafruit_PWMServoDriver servo_controller = Adafruit_PWMServoDriver(PCA9685_ADDRESS);

		//this stores the order of the led positions on the lighting controller that correspond to the oMIDItone head.
		uint16_t * led_position_array;
		
		//this is to measure the frequency of rising edges produced by the output sound wave
		elapsedMicros last_rising_edge;

		//this is to make sure the frequency correction isn't happening faster than the digital pots can be set
		elapsedMillis last_adjust_time;

		//this is to make sure the frequency correction isn't happening faster than the digital pots can be set
		elapsedMillis last_stabilize_time;

		//This is a time tracker that will force a short pause between notes.
		elapsedMillis note_start_time;

		//this is to allow the oMIDItone to play a note for a bit before it sets the initial midi_freqs
		elapsedMillis startup_stabilization_began;

		//this is to allow the oMIDItone to play a note for a bit before it sets the initial midi_freqs
		elapsedMillis last_freq_measurement;

		//This tracks when the previous servo update ran on this head
		elapsedMillis last_servo_update;
};

#endif
