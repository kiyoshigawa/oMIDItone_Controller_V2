/*
oMIDItone_Controller V2
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

#include <Arduino.h>
#include <ADC.h>
#include <Adafruit_NeoPixel.h>
#include <colors.h>
#include <lighting_control.h>
#include <MIDIController.h>
#include <oMIDItone.h>

//this will print messages on system startup and init
#define OMIDITONE_DEBUG

//This will turn on/off the debug note messages when notes are added or removed via MIDI
//#define NOTE_DEBUG

//defines to make the head selection code more readable:
#define AVAILABLE true
#define NOT_AVAILABLE false

//This is the pulse offset in microseconds from the min value of the servo position to the max (i.e. fully open)
#define SERVO_MAX_OFFSET 220

//The next few #defines are for the Adafruit_Neopixel.h library stuff:
#define LED_TOTAL_NUM NUM_LEDS_PER_HEAD*NUM_OMIDITONES
#define LED_DATA_PIN 2
#define LED_COLOR_ORDER NEO_GRB
#define LED_SPEED NEO_KHZ800
#define LED_BRIGHTNESS 255

//these next several #defines are default lighting info for the heads:
#define DEFAULT_BG_MODE lc_bg::rainbow_slow_rotate
#define DEFAULT_FG_MODE lc_fg::none
#define DEFAULT_TRIGGER_MODE lc_trigger::color_pulse

#define DEFAULT_OM1_BG_RAINBOW 16
#define DEFAULT_OM2_BG_RAINBOW 17
#define DEFAULT_OM3_BG_RAINBOW 18
#define DEFAULT_OM4_BG_RAINBOW 19
#define DEFAULT_OM5_BG_RAINBOW 20
#define DEFAULT_OM6_BG_RAINBOW 21
#define DEFAULT_OM1_FG_RAINBOW 1
#define DEFAULT_OM2_FG_RAINBOW 1
#define DEFAULT_OM3_FG_RAINBOW 1
#define DEFAULT_OM4_FG_RAINBOW 1
#define DEFAULT_OM5_FG_RAINBOW 1
#define DEFAULT_OM6_FG_RAINBOW 1
#define DEFAULT_OM1_TRIGGER_RAINBOW 3
#define DEFAULT_OM2_TRIGGER_RAINBOW 5
#define DEFAULT_OM3_TRIGGER_RAINBOW 9
#define DEFAULT_OM4_TRIGGER_RAINBOW 13
#define DEFAULT_OM5_TRIGGER_RAINBOW 7
#define DEFAULT_OM6_TRIGGER_RAINBOW 11

#define DEFAULT_NOTE_TRIGGER_SETTING true
#define DEFAULT_LIGHTING_ENABLED_SETTING true

//this allows me to reset the teensy when it receives a MIDI CC121 reset command.
#define SCB_AIRCR (*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control location

void _softRestart(void)
{
	Serial.end();  //clears the serial monitor  if used
	SCB_AIRCR = 0x05FA0004;  //write value for restart
}

//this is the MIDIController object that keeps track of the MIDI information so we can assign heads to play notes properly:
MIDIController mc = MIDIController();

//this controls whether lighting is enabled or not
bool lighting_is_enabled = DEFAULT_LIGHTING_ENABLED_SETTING;

//this controls whether note on messages send triggers to the head that plays the note:
bool note_trigger_is_enabled = DEFAULT_NOTE_TRIGGER_SETTING;

// Pin and other head-specific Definitions
//om#_leds[] arrays are per head ordered from left to right, the first 6 are front leds, the next 6 are the back top, and the final 6 are the back bottom leds
//Red Head:
const uint16_t om1_se_pin = 15;
const uint16_t om1_sd_pin = 26;
const uint16_t om1_cs1_pin = 17;
const uint16_t om1_cs2_pin = 16;
const uint16_t om1_analog_feedback_pin = A10;
const uint16_t om1_l_min = 1390;
const uint16_t om1_l_max = om1_l_min + SERVO_MAX_OFFSET;
const uint16_t om1_r_min = 1340;
const uint16_t om1_r_max = om1_r_min - SERVO_MAX_OFFSET;
const uint16_t om1_l_channel = 0;
const uint16_t om1_r_channel = 1;
uint16_t om1_leds[NUM_LEDS_PER_HEAD] = {12, 13, 14, 15, 16, 17, 95, 94, 93, 92, 91, 90, 84, 85, 86, 87, 88, 89};
//Yellow Head:
const uint16_t om2_se_pin = 33;
const uint16_t om2_sd_pin = 25;
const uint16_t om2_cs1_pin = 22;
const uint16_t om2_cs2_pin = 21;
const uint16_t om2_analog_feedback_pin = A11;
const uint16_t om2_l_min = 1475;
const uint16_t om2_l_max = om1_l_min + SERVO_MAX_OFFSET;
const uint16_t om2_r_min = 1430;
const uint16_t om2_r_max = om1_r_min - SERVO_MAX_OFFSET;
const uint16_t om2_l_channel = 2;
const uint16_t om2_r_channel = 3;
uint16_t om2_leds[NUM_LEDS_PER_HEAD] = {23, 22, 21, 20, 19, 18, 59, 58, 57, 56, 55, 54, 48, 49, 50, 51, 52, 53};
//White Head:
const uint16_t om3_se_pin = 24;
const uint16_t om3_sd_pin = 30;
const uint16_t om3_cs1_pin = 3;
const uint16_t om3_cs2_pin = 23;
const uint16_t om3_analog_feedback_pin = A13;
const uint16_t om3_l_min = 1280;
const uint16_t om3_l_max = om1_l_min + SERVO_MAX_OFFSET;
const uint16_t om3_r_min = 1450;
const uint16_t om3_r_max = om1_r_min - SERVO_MAX_OFFSET;
const uint16_t om3_l_channel = 4;
const uint16_t om3_r_channel = 5;
uint16_t om3_leds[NUM_LEDS_PER_HEAD] = {6, 7, 8, 9, 10, 11, 101, 100, 99, 98, 97, 96, 78, 79, 80, 81, 82, 83};
//Black Head:
const uint16_t om4_se_pin = 10;
const uint16_t om4_sd_pin = 29;
const uint16_t om4_cs1_pin = 9;
const uint16_t om4_cs2_pin = 8;
const uint16_t om4_analog_feedback_pin = A12;
const uint16_t om4_l_min = 1475;
const uint16_t om4_l_max = om1_l_min + SERVO_MAX_OFFSET;
const uint16_t om4_r_min = 1310;
const uint16_t om4_r_max = om1_r_min - SERVO_MAX_OFFSET;
const uint16_t om4_l_channel = 6;
const uint16_t om4_r_channel = 7;
uint16_t om4_leds[NUM_LEDS_PER_HEAD] = {29, 28, 27, 26, 25, 24, 65, 64, 63, 62, 61, 60, 42, 43, 44, 45, 46, 47};
//Green Head:
const uint16_t om5_se_pin = 32;
const uint16_t om5_sd_pin = 28;
const uint16_t om5_cs1_pin = 7;
const uint16_t om5_cs2_pin = 6;
const uint16_t om5_analog_feedback_pin = A14;
const uint16_t om5_l_min = 1475;
const uint16_t om5_l_max = om1_l_min + SERVO_MAX_OFFSET;
const uint16_t om5_r_min = 1420;
const uint16_t om5_r_max = om1_r_min - SERVO_MAX_OFFSET;
const uint16_t om5_l_channel = 8;
const uint16_t om5_r_channel = 9;
uint16_t om5_leds[NUM_LEDS_PER_HEAD] = {0, 1, 2, 3, 4, 5, 107, 106, 105, 104, 103, 102, 72, 73, 74, 75, 76, 77};
//Blue Head:
const uint16_t om6_se_pin = 31;
const uint16_t om6_sd_pin = 27;
const uint16_t om6_cs1_pin = 5;
const uint16_t om6_cs2_pin = 4;
const uint16_t om6_analog_feedback_pin = A0;
const uint16_t om6_l_min = 1335;
const uint16_t om6_l_max = om1_l_min + SERVO_MAX_OFFSET;
const uint16_t om6_r_min = 1410;
const uint16_t om6_r_max = om1_r_min - SERVO_MAX_OFFSET;
const uint16_t om6_l_channel = 14;
const uint16_t om6_r_channel = 15;
uint16_t om6_leds[NUM_LEDS_PER_HEAD] = {35, 34, 33, 32, 31, 30, 71, 70, 69, 68, 67, 66, 36, 37, 38, 39, 40, 41};

//this array controls the type of notes being sent to each head on note_trigger events when they are active:
uint16_t note_trigger_type[NUM_OMIDITONES] = {	
	DEFAULT_TRIGGER_MODE,
	DEFAULT_TRIGGER_MODE,
	DEFAULT_TRIGGER_MODE,
	DEFAULT_TRIGGER_MODE,
	DEFAULT_TRIGGER_MODE,
	DEFAULT_TRIGGER_MODE
};

//these are the Animation objects for the heads above - they will control the animation per-head.
Animation om1_animation = Animation(
	om1_leds, NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM1_BG_RAINBOW],
	rb_array[DEFAULT_OM1_FG_RAINBOW],
	rb_array[DEFAULT_OM1_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);
Animation om2_animation = Animation(
	om2_leds, NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM2_BG_RAINBOW],
	rb_array[DEFAULT_OM2_FG_RAINBOW],
	rb_array[DEFAULT_OM2_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);
Animation om3_animation = Animation(
	om3_leds, NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM3_BG_RAINBOW],
	rb_array[DEFAULT_OM3_FG_RAINBOW],
	rb_array[DEFAULT_OM3_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);
Animation om4_animation = Animation(
	om4_leds, NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM4_BG_RAINBOW],
	rb_array[DEFAULT_OM4_FG_RAINBOW],
	rb_array[DEFAULT_OM4_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);
Animation om5_animation = Animation(
	om5_leds, NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM5_BG_RAINBOW],
	rb_array[DEFAULT_OM5_FG_RAINBOW],
	rb_array[DEFAULT_OM5_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);
Animation om6_animation = Animation(
	om6_leds, NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM6_BG_RAINBOW],
	rb_array[DEFAULT_OM6_FG_RAINBOW],
	rb_array[DEFAULT_OM6_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);

//This is the LED strip object to control all the lighting on the board
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_TOTAL_NUM, LED_DATA_PIN, LED_COLOR_ORDER + LED_SPEED);

//this is the lighting controller that will control the lighting updates on the above Adafruit_NeoPixel object's pixels:
LightingControl lc = LightingControl(&strip, LED_BRIGHTNESS);

//this is a timing variable to track lighting updates
elapsedMicros last_lighting_update;

//Using SPI0 on board, MOSI0 = 11, MISO0 = 12, and SCK0 = 13, which will blink the LED as it sends.

//declare the oMIDItone objects:
oMIDItone oms[NUM_OMIDITONES] = {
	oMIDItone(	
		om1_se_pin, 
		om1_sd_pin, 
		om1_cs1_pin, 
		om1_cs2_pin, 
		om1_analog_feedback_pin, 
		om1_l_channel, 
		om1_r_channel, 
		om1_l_min, 
		om1_l_max, 
		om1_r_min, 
		om1_r_max, 
		om1_leds, 
		&om1_animation),
	oMIDItone(
		om2_se_pin, 
		om2_sd_pin, 
		om2_cs1_pin, 
		om2_cs2_pin, 
		om2_analog_feedback_pin, 
		om2_l_channel, 
		om2_r_channel, 
		om2_l_min, 
		om2_l_max, 
		om2_r_min, 
		om2_r_max, 
		om2_leds, 
		&om2_animation),
	oMIDItone(
		om3_se_pin, 
		om3_sd_pin, 
		om3_cs1_pin, 
		om3_cs2_pin, 
		om3_analog_feedback_pin, 
		om3_l_channel, 
		om3_r_channel, 
		om3_l_min, 
		om3_l_max, 
		om3_r_min, 
		om3_r_max, 
		om3_leds, 
		&om3_animation),
	oMIDItone(
		om4_se_pin, 
		om4_sd_pin, 
		om4_cs1_pin, 
		om4_cs2_pin, 
		om4_analog_feedback_pin, 
		om4_l_channel, 
		om4_r_channel, 
		om4_l_min, 
		om4_l_max, 
		om4_r_min, 
		om4_r_max, 
		om4_leds, 
		&om4_animation),
	oMIDItone(
		om5_se_pin, 
		om5_sd_pin, 
		om5_cs1_pin, 
		om5_cs2_pin, 
		om5_analog_feedback_pin, 
		om5_l_channel, 
		om5_r_channel, 
		om5_l_min, 
		om5_l_max, 
		om5_r_min, 
		om5_r_max, 
		om5_leds, 
		&om5_animation),
	oMIDItone(
		om6_se_pin, 
		om6_sd_pin, 
		om6_cs1_pin, 
		om6_cs2_pin, 
		om6_analog_feedback_pin, 
		om6_l_channel, 
		om6_r_channel, 
		om6_l_min, 
		om6_l_max, 
		om6_r_min, 
		om6_r_max, 
		om6_leds, 
		&om6_animation),
};

//TIM:
//need to make all the MIDI CC handler functions for oMIDItone specific tasks,
//such as lighting functions and animations.

//This tracks the current head so the iteration isn't always in the same place.
int head_offset = 0;

//this is an array that is sorted in the order of the least recently used heads to the most recently used heads
//it allows for the least recently used heads to be selected first when deciding whish head will play a note
uint8_t head_order_array[NUM_OMIDITONES];

//this stores the state of the head order array until all notes are off, then it is pushed into the head order array.
uint8_t pending_head_order_array[NUM_OMIDITONES];

//this function moves the head in question to the end of the head_order_array, and moves the remaining heads down.
void pending_head_order_to_end(uint8_t head_number)
{
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

//this iterates through the MIDIController's current note array and assign heads to play the notes
void update_oMIDItones(void)
{
	//check to see if a tune request was received. If so, call the init function for the heads
	//this will take a while, so don't automate it into your MIDI files plsthx
	if(mc.tune_request_was_received()){
		for(int i=0; i<NUM_OMIDITONES; i++){
			oms[i].init();
		}
	}

	//check to see if a system reset request was received. This will reset the entire teensy,
	if(mc.system_reset_request_was_received()){
		_softRestart();
	}

	//Now we need to handle notes messages:
	//TIM: figure out a sane way to assign notes to heads
	//keep in mind notes can be added, removed, or changed
	//and different actions will be needed for each of these options.
}

//this function callc the lc.update() function whenever lighting is enabled, and marks the head tunings as invalid after
void update_lighting(void)
{
	if(lighting_is_enabled){
		int lighting_data_was_sent = lc.update();
		if(lighting_data_was_sent == LC_STRIP_WRITTEN){
			for(int h=0; h<NUM_OMIDITONES; h++){
				oms[h].cancel_pitch_correction();
			}
		}
	}
}

void setup(void)
{
	#ifdef OMIDITONE_DEBUG
		Serial.begin(9600);
		while(!Serial){
			if(millis() > 5000){
				//if the USB Serial can't get it's shit together in 5 seconds, move on without it.
				//this will break debug if it gets to this point
				break;
			}
		}
		Serial.println("Welcome to oMIDItone.");
		Serial.println("Beginning initialization - this may take several minutes...");
	#endif
	
	//set up the lighting controller with the animations
	lc.init();
	lc.update();

	//initialize the per-head stuff:
	for(int i=0; i<NUM_OMIDITONES; i++){
		head_order_array[i] = i;
		pending_head_order_array[i] = i;
		lc.add_animation(oms[i].animation);
		//init the om objects - This is going to take a while - like several minutes:
		oms[i].init();
		lc.update();
	}

	//initialize the MIDIController:
	mc.init();

	//assign MIDI CC handler functions as needed
	//mc.assign_cc_handler(cc_number, function_name);

	#ifdef OMIDITONE_DEBUG
		Serial.println("Init Complete, awaiting MIDI input.");
	#endif
}

void loop(void)
{
	//this will update the MIDI info on the controller to be current for use below:
	mc.update();

	//This will decide which oMIDItone to use for which note, and assign them to play
	update_oMIDItones();

	//this will update all lighting functions on a regular basis
	update_lighting();
}

/* Old code that needs to be reintegrated or replaced...



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