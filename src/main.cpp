/*
oMIDItone_Controller V2

This is the code that runs the oMIDItone. The oMIDItone is an analog synthesizer
that is built out of six heavily modified Otamatones, being controller by a 
Teensy 3.2 and some custom hardware. Throughout these docs, I will refer to the
individual Otamatones as 'heads', which each will have animations and are able
to play frequencies. The comments throughout the code go over the specifics of 
how everything works, but I will provide a general overview here:

The controller can be broken down into four distinct functions, each of which
is run by a custom library built for this project:
1. MIDIController.h
2. oMIDItone.h
3. lighting_control.h
4. main.cpp

MIDIController.h:
	This library is designed to take all incoming MIDI messages and keep a 
	'state-of-the-MIDI' class controller object updated with a list of all
	current playing MIDI notes and to handle any incoming MIDI CC messages as
	needed. I've got this set up to decide not only the desired note frequencies
	that are going to be played by the oMIDItone, but also to control lighting
	and servo animations for the heads using MIDI CC messages. This is done 
	through custom user-defined functions in my main.cpp file for the oMIDItone.

oMIDItone.h:
	This is the code that allows me to interface with the Otamatone's analog
	synth hardware circuit and control it digitally. It lets me set what desired
	frequency I want a head to play, as well as controls the mouth animation for
	the servos on that head. It emplys the Teensy's analog pins for feedback, 
	and it also can shut off sound to the heads when desired.

lighting_control.h:
	This controls lighting animations for an addressable LED strip that shines 
	in front of and behind the heads. Each head has 18 LEDs assigned to it, for 
	a total of 108 LEDs. The hardware interface is handled by the 
	Adafruit_NeoPixel library, and the lighting controller handles 6 animations,
	one for each head. The animations are all individually controllable with 
	MIDI messages, and each head's animation can be configured independently.

main.cpp:
	This file ties the three main sub-libraries described above together and 
	decides how to translate incoming MIDI messages into outputting actual 
	sounds and lighting modes to the six heads in the project. 
	The program runs in an entirely serial, non-blocking manner, starting with a
	single-run setup() function that runs once on power-up or hard reset.

	The setup function and all the definitions above's main purpose is to assign
	the correct pins to the class objects so they will function, and initialize
	everything to a default value. In the case of the oMIDItone.h, the init 
	function contains a lengthy tuning process which will determien what 
	frequency ranges each head is capable of playing. This also initializes the
	USB and hardware MIDI so that the MIDIController.h class object is ready to
	receive any incoming messages. Finally, this starts the oMIDItone with a 
	default animation mode for every head.
	
	Once the hardware setup is complete, it runs a loop() function endlessly 
	which will:
		1. Update the MIDIController object to generate a current list of notes 
		and handle any control messages. These drive all output of the oMIDItone
		and without a MIDI signal being input, either via hardware or USB, the 
		oMIDItone will not play any music.
		2. Take the MIDI information and assign heads to play the notes based on
		the order they were received, as well as note frequency. This will also
		make the mouths open if the head is playing a note.
		3. Take the MIDI information and update any lighting effects based on if
		note triggering is enabled (creates a transient lighting effect based on
		when a new note is added or changed), or based on CC messages 
		specifically assigned to change lighting features.
		4. Update the lighting controller, which will push these lighting 
		effects to the LEDs causing them to be displayed.

The oMIDItone hardware runs a custom board that is designed to connect the 
original Otamatone analog synth PCB to a teensy and allow direct control. All 
hardware designs of both the original baord that I reverse engineered, and my
controller PCB that runs the oMIDItone are available in the 'Controller PCB' 
directory of this repo.

Note that the V2 hardware had some issues that I fixed manually, so making a PCB
directly from the files provided is not advised. I plan to eventually release an
updated hardware revision that should address these concerns, but I have not 
gotten around to it yet, as my fixes worked and the board I have now is 
functional enough for the project at this time.

For more information on the specific outstanding hardware issues, check the main
repo readme.md file.

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

//TIM: remove this function once all the crashing is figured out.
uint32_t free_mem(){ // for Teensy 3.0
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is (approximately) the free, available ram.
    return stackTop - heapTop;
}

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
#define LED_TOTAL_NUM OM_NUM_LEDS_PER_HEAD*OM_NUM_OMIDITONES
#define LED_DATA_PIN 2
#define LED_COLOR_ORDER NEO_GRB
#define LED_SPEED NEO_KHZ800
#define LED_BRIGHTNESS 255

//these next several #defines are default lighting info for the heads:
#define DEFAULT_BG_MODE lc_bg::rainbow_slow_rotate
#define DEFAULT_FG_MODE lc_fg::no_fg
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
	Serial.println("Hard reset request received. Restarting in 5 seconds...");
	delay(5000);
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
uint16_t om1_leds[OM_NUM_LEDS_PER_HEAD] = {12, 13, 14, 15, 16, 17, 95, 94, 93, 92, 91, 90, 84, 85, 86, 87, 88, 89};
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
uint16_t om2_leds[OM_NUM_LEDS_PER_HEAD] = {23, 22, 21, 20, 19, 18, 59, 58, 57, 56, 55, 54, 48, 49, 50, 51, 52, 53};
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
uint16_t om3_leds[OM_NUM_LEDS_PER_HEAD] = {6, 7, 8, 9, 10, 11, 101, 100, 99, 98, 97, 96, 78, 79, 80, 81, 82, 83};
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
uint16_t om4_leds[OM_NUM_LEDS_PER_HEAD] = {29, 28, 27, 26, 25, 24, 65, 64, 63, 62, 61, 60, 42, 43, 44, 45, 46, 47};
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
uint16_t om5_leds[OM_NUM_LEDS_PER_HEAD] = {0, 1, 2, 3, 4, 5, 107, 106, 105, 104, 103, 102, 72, 73, 74, 75, 76, 77};
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
uint16_t om6_leds[OM_NUM_LEDS_PER_HEAD] = {35, 34, 33, 32, 31, 30, 71, 70, 69, 68, 67, 66, 36, 37, 38, 39, 40, 41};

//this array controls the type of notes being sent to each head on note_trigger events when they are active:
uint16_t note_trigger_type[OM_NUM_OMIDITONES] = {	
	DEFAULT_TRIGGER_MODE,
	DEFAULT_TRIGGER_MODE,
	DEFAULT_TRIGGER_MODE,
	DEFAULT_TRIGGER_MODE,
	DEFAULT_TRIGGER_MODE,
	DEFAULT_TRIGGER_MODE
};

//these are the Animation objects for the heads above - they will control the animation per-head.
Animation om1_animation = Animation(
	om1_leds, OM_NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM1_BG_RAINBOW],
	rb_array[DEFAULT_OM1_FG_RAINBOW],
	rb_array[DEFAULT_OM1_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);
Animation om2_animation = Animation(
	om2_leds, OM_NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM2_BG_RAINBOW],
	rb_array[DEFAULT_OM2_FG_RAINBOW],
	rb_array[DEFAULT_OM2_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);
Animation om3_animation = Animation(
	om3_leds, OM_NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM3_BG_RAINBOW],
	rb_array[DEFAULT_OM3_FG_RAINBOW],
	rb_array[DEFAULT_OM3_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);
Animation om4_animation = Animation(
	om4_leds, OM_NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM4_BG_RAINBOW],
	rb_array[DEFAULT_OM4_FG_RAINBOW],
	rb_array[DEFAULT_OM4_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);
Animation om5_animation = Animation(
	om5_leds, OM_NUM_LEDS_PER_HEAD,
	(DEFAULT_BG_MODE | DEFAULT_FG_MODE),
	rb_array[DEFAULT_OM5_BG_RAINBOW],
	rb_array[DEFAULT_OM5_FG_RAINBOW],
	rb_array[DEFAULT_OM5_TRIGGER_RAINBOW],
	LC_DEFAULT_REFRESH_RATE
	);
Animation om6_animation = Animation(
	om6_leds, OM_NUM_LEDS_PER_HEAD,
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
oMIDItone oms[OM_NUM_OMIDITONES] = {
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

//a quick check to make sure a number corresponds to a valid rainbow in the rb_array
//if the number is larger than the num_rainbows, it returns 0, otherwise it returns value
uint8_t validate_rainbow_number(uint8_t value)
{
	if(value > num_rainbows){
		return 0;
	} else {
		return value;
	}
}

//the next 55 functions are the CC handlers for lighting effects, servo 
//positions, pitch correction, note triggering, etc. that are called 
//automatically by the MIDIController when received during an update.
//They need to be assigned in the setup() function.

void handle_cc_9_hard_reset(uint8_t channel, uint8_t cc_value)
{
	_softRestart();
}

void handle_cc_14_pitch_correction_toggle(uint8_t channel, uint8_t cc_value)
{
	if(cc_value == 0){
		for(int i=0; i<OM_NUM_OMIDITONES; i++){
			oms[i].disable_pitch_correction();
		}
	} else {
		for(int i=0; i<OM_NUM_OMIDITONES; i++){
			oms[i].enable_pitch_correction();
		}
	}
}

void handle_cc_15_note_trigger_toggle(uint8_t channel, uint8_t cc_value)
{
	if(cc_value == 0){
		note_trigger_is_enabled = false;
	} else {
		note_trigger_is_enabled = true;
	}
}

void handle_cc_20_lighting_toggle(uint8_t channel, uint8_t cc_value)
{
	if(cc_value == 0){
		lighting_is_enabled = false;
	} else {
		lighting_is_enabled = true;
	}
}

void handle_cc_21_servo_toggle(uint8_t channel, uint8_t cc_value)
{
	if(cc_value == 0){
		for(int i=0; i<OM_NUM_OMIDITONES; i++){
			oms[i].disable_servos();
		}
	} else {
		for(int i=0; i<OM_NUM_OMIDITONES; i++){
			oms[i].enable_servos();
		}
	}
}

void handle_cc_22_om1_bg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[0].animation->change_rainbow(LC_BG, rb_array[cc_value]);
}

void handle_cc_23_om2_bg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[1].animation->change_rainbow(LC_BG, rb_array[cc_value]);
}

void handle_cc_24_om3_bg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[2].animation->change_rainbow(LC_BG, rb_array[cc_value]);
}

void handle_cc_25_om4_bg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[3].animation->change_rainbow(LC_BG, rb_array[cc_value]);
}

void handle_cc_26_om5_bg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[4].animation->change_rainbow(LC_BG, rb_array[cc_value]);
}

void handle_cc_27_om6_bg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[5].animation->change_rainbow(LC_BG, rb_array[cc_value]);
}

void handle_cc_28_om1_bg_change(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[0].animation->validate_lc_bg(cc_value);
	uint16_t lm = oms[0].animation->current_fg_mode() | cc_value;
	oms[0].animation->change_lighting_mode(lm);
}

void handle_cc_29_om2_bg_change(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[1].animation->validate_lc_bg(cc_value);
	uint16_t lm = oms[1].animation->current_fg_mode() | cc_value;
	oms[1].animation->change_lighting_mode(lm);
}

void handle_cc_30_om3_bg_change(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[2].animation->validate_lc_bg(cc_value);
	uint16_t lm = oms[2].animation->current_fg_mode() | cc_value;
	oms[2].animation->change_lighting_mode(lm);
}

void handle_cc_31_om4_bg_change(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[3].animation->validate_lc_bg(cc_value);
	uint16_t lm = oms[3].animation->current_fg_mode() | cc_value;
	oms[3].animation->change_lighting_mode(lm);
}
void handle_cc_41_om1_servo_pos(uint8_t channel, uint8_t cc_value)
{
	oms[0].set_servos(cc_value);
}
void handle_cc_46_om5_bg_change(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[4].animation->validate_lc_bg(cc_value);
	uint16_t lm = oms[4].animation->current_fg_mode() | cc_value;
	oms[4].animation->change_lighting_mode(lm);
}
	
void handle_cc_47_om6_bg_change(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[5].animation->validate_lc_bg(cc_value);
	uint16_t lm = oms[5].animation->current_fg_mode() | cc_value;
	oms[5].animation->change_lighting_mode(lm);
}
	
void handle_cc_52_om1_fg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[0].animation->change_rainbow(LC_FG, rb_array[cc_value]);
}
	
void handle_cc_53_om2_fg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[1].animation->change_rainbow(LC_FG, rb_array[cc_value]);
}
	
void handle_cc_54_om3_fg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[2].animation->change_rainbow(LC_FG, rb_array[cc_value]);
}
	
void handle_cc_55_om4_fg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[3].animation->change_rainbow(LC_FG, rb_array[cc_value]);
}
	
void handle_cc_56_om5_fg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[4].animation->change_rainbow(LC_FG, rb_array[cc_value]);
}
	
void handle_cc_57_om6_fg_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[5].animation->change_rainbow(LC_FG, rb_array[cc_value]);
}
	
void handle_cc_58_om1_fg_change(uint8_t channel, uint8_t cc_value)
{
	uint16_t shifted_value = cc_value << 8; //since MIDI is only 7 bit, needed to offset manually to get correct values over MIDI commands
	shifted_value = oms[0].animation->validate_lc_fg(shifted_value);
	uint16_t lm = oms[0].animation->current_bg_mode() | shifted_value;
	oms[0].animation->change_lighting_mode(lm);
}

void handle_cc_59_om2_fg_change(uint8_t channel, uint8_t cc_value)
{
	uint16_t shifted_value = cc_value << 8; //since MIDI is only 7 bit, needed to offset manually to get correct values over MIDI commands
	shifted_value = oms[1].animation->validate_lc_fg(shifted_value);
	uint16_t lm = oms[1].animation->current_bg_mode() | shifted_value;
	oms[1].animation->change_lighting_mode(lm);
}
	
void handle_cc_60_om3_fg_change(uint8_t channel, uint8_t cc_value)
{
	uint16_t shifted_value = cc_value << 8; //since MIDI is only 7 bit, needed to offset manually to get correct values over MIDI commands
	shifted_value = oms[2].animation->validate_lc_fg(shifted_value);
	uint16_t lm = oms[2].animation->current_bg_mode() | shifted_value;
	oms[2].animation->change_lighting_mode(lm);
}
	
void handle_cc_61_om4_fg_change(uint8_t channel, uint8_t cc_value)
{
	uint16_t shifted_value = cc_value << 8; //since MIDI is only 7 bit, needed to offset manually to get correct values over MIDI commands
	shifted_value = oms[3].animation->validate_lc_fg(shifted_value);
	uint16_t lm = oms[3].animation->current_bg_mode() | shifted_value;
	oms[3].animation->change_lighting_mode(lm);
}
	
void handle_cc_62_om5_fg_change(uint8_t channel, uint8_t cc_value)
{
	uint16_t shifted_value = cc_value << 8; //since MIDI is only 7 bit, needed to offset manually to get correct values over MIDI commands
	shifted_value = oms[4].animation->validate_lc_fg(shifted_value);
	uint16_t lm = oms[4].animation->current_bg_mode() | shifted_value;
	oms[4].animation->change_lighting_mode(lm);
}
	
void handle_cc_63_om6_fg_change(uint8_t channel, uint8_t cc_value)
{
	uint16_t shifted_value = cc_value << 8; //since MIDI is only 7 bit, needed to offset manually to get correct values over MIDI commands
	shifted_value = oms[5].animation->validate_lc_fg(shifted_value);
	uint16_t lm = oms[5].animation->current_bg_mode() | shifted_value;
	oms[5].animation->change_lighting_mode(lm);
}
	
void handle_cc_85_om2_servo_pos(uint8_t channel, uint8_t cc_value)
{
	oms[1].set_servos(cc_value);
}
	
void handle_cc_86_om3_servo_pos(uint8_t channel, uint8_t cc_value)
{
	oms[2].set_servos(cc_value);
	
}
void handle_cc_87_om4_servo_pos(uint8_t channel, uint8_t cc_value)
{
	oms[3].set_servos(cc_value);
	
}
void handle_cc_89_om5_servo_pos(uint8_t channel, uint8_t cc_value)
{
	oms[4].set_servos(cc_value);
	
}
void handle_cc_90_om6_servo_pos(uint8_t channel, uint8_t cc_value)
{
	oms[5].set_servos(cc_value);
	
}
void handle_cc_102_om1_trigger_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[0].animation->change_rainbow(LC_TRIGGER, rb_array[cc_value]);
	
}
void handle_cc_103_om2_trigger_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[1].animation->change_rainbow(LC_TRIGGER, rb_array[cc_value]);
}
	
void handle_cc_104_om3_trigger_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[2].animation->change_rainbow(LC_TRIGGER, rb_array[cc_value]);
}
	
void handle_cc_105_om4_trigger_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[3].animation->change_rainbow(LC_TRIGGER, rb_array[cc_value]);
}
	
void handle_cc_106_om5_trigger_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[4].animation->change_rainbow(LC_TRIGGER, rb_array[cc_value]);
}
	
void handle_cc_107_om6_trigger_rainbow(uint8_t channel, uint8_t cc_value)
{
	cc_value = validate_rainbow_number(cc_value);
	oms[5].animation->change_rainbow(LC_TRIGGER, rb_array[cc_value]);
}
	
void handle_cc_108_om1_trigger(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[0].animation->validate_lc_trigger(cc_value);
	oms[0].animation->trigger_event(cc_value);
}

void handle_cc_109_om2_trigger(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[1].animation->validate_lc_trigger(cc_value);
	oms[1].animation->trigger_event(cc_value);
}
	
void handle_cc_110_om3_trigger(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[2].animation->validate_lc_trigger(cc_value);
	oms[2].animation->trigger_event(cc_value);
}
	
void handle_cc_111_om4_trigger(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[3].animation->validate_lc_trigger(cc_value);
	oms[3].animation->trigger_event(cc_value);
}
	
void handle_cc_112_om5_trigger(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[4].animation->validate_lc_trigger(cc_value);
	oms[4].animation->trigger_event(cc_value);
}
	
void handle_cc_113_om6_trigger(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[5].animation->validate_lc_trigger(cc_value);
	oms[5].animation->trigger_event(cc_value);
}
	
void handle_cc_114_om1_note_trigger_type(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[0].animation->validate_lc_trigger(cc_value);
	note_trigger_type[0] = cc_value;
}

void handle_cc_115_om2_note_trigger_type(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[1].animation->validate_lc_trigger(cc_value);
	note_trigger_type[1] = cc_value;
}
	
void handle_cc_116_om3_note_trigger_type(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[2].animation->validate_lc_trigger(cc_value);
	note_trigger_type[2] = cc_value;
}
	
void handle_cc_117_om4_note_trigger_type(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[3].animation->validate_lc_trigger(cc_value);
	note_trigger_type[3] = cc_value;
}
	
void handle_cc_118_om5_note_trigger_type(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[4].animation->validate_lc_trigger(cc_value);
	note_trigger_type[4] = cc_value;
}
	
void handle_cc_119_om6_note_trigger_type(uint8_t channel, uint8_t cc_value)
{
	cc_value = oms[5].animation->validate_lc_trigger(cc_value);
	note_trigger_type[5] = cc_value;
}

//this tracks which heads are free to play notes and which have alreayd been assigned a note.
bool is_head_available_array[OM_NUM_OMIDITONES];

//this tracks the note number currently being played on a head
uint8_t head_note_array[OM_NUM_OMIDITONES];

//this tracks the channel number of the note currently being played on a head
uint8_t head_channel_array[OM_NUM_OMIDITONES];

//This tracks the current head so the iteration isn't always in the same place.
int head_offset = 0;

//this is an array that is sorted in the order of the least recently used heads to the most recently used heads
//it allows for the least recently used heads to be selected first when deciding whish head will play a note
uint8_t head_order_array[OM_NUM_OMIDITONES];

//this stores the state of the head order array until all notes are off, then it is pushed into the head order array.
uint8_t pending_head_order_array[OM_NUM_OMIDITONES];

//this function moves the head in question to the end of the head_order_array, and moves the remaining heads down.
void pending_head_order_to_end(uint8_t head_number)
{
	//don't need to iterate to the last one, because if the head_number is in the last position already, we're good
	for(int i=0; i<OM_NUM_OMIDITONES-1; i++){
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
		for(int i=0; i<OM_NUM_OMIDITONES; i++){
			oms[i].init();
		}
	}

	//check to see if a system reset request was received. This will reset the entire teensy,
	if(mc.system_reset_request_was_received()){
		_softRestart();
	}

	//get the initial status for note changes:
	bool note_was_added = mc.note_was_added();
	bool note_was_changed = mc.note_was_changed();
	bool note_was_removed = mc.note_was_removed();

	//make sure no heads have dropped a note
	for(int h=0; h<OM_NUM_OMIDITONES; h++){
		if(oms[h].note_was_dropped()){
			//if a head dropped a note, we need to reassign notes:
			note_was_added = true;
		}
	}

	//check to see if any currently playing notes have changed and update them first.
	if(note_was_changed){
		//check to see if a currently-playing note changed and update the head frequency.
		for(int h=0; h<OM_NUM_OMIDITONES; h++){
			//if the head isn't playing a note, break:
			if(oms[h].is_ready()){
				//don't change anything
			} else {
				//check to see if the note the head is currently playing is no longer in the array
				uint8_t note_position = mc.check_note(head_channel_array[h], head_note_array[h]);
				if(note_position != MIDI_NO_NOTE){
					bool can_play_new_freq = oms[h].update_freq(mc.current_notes[note_position].freq);
					if(!can_play_new_freq){
						//if it can't play the updated frequency, force the heads to reassign all notes.
						//this will make sure pitch bends don't get lost at the edges of what heads can play.
						note_was_added = true;
					}
					#ifdef NOTE_DEBUG
							Serial.print("H");
							Serial.print(head_order_array[h]);
							Serial.print(": Note ");
							Serial.print(head_note_array[h]);
							Serial.println(" changed.");
					#endif
				}
			}
		}
	}
	
	//if a note was added or removed, we need to reassign all the heads:
	if(note_was_added || note_was_removed){
		//need to track how many heads have been assigned notes:
		uint8_t num_assigned_heads = 0;
		//we need to make all heads available and clear their current notes
		for(int h=0; h<OM_NUM_OMIDITONES; h++){
			is_head_available_array[h] = true;
			head_note_array[h] = MIDI_NO_NOTE;
			head_channel_array[h] = MIDI_NO_CHANNEL;
		}
		//if there are no notes, all heads to sound off.
		if(mc.num_current_notes == 0){
			for(int h=0; h<OM_NUM_OMIDITONES; h++){
				oms[head_order_array[h]].sound_off();
				//only update the head order array if no notes are playing
				head_order_array[h] = pending_head_order_array[h];
			}
		}
		//iterate through the mc.current_notes[] array from last to first:
		for(int n=mc.num_current_notes-1; n>=0; n--){
			//check each head to see if it can play the note:
			for(int h=0; h<OM_NUM_OMIDITONES; h++){
				//if the head can play the note
				uint8_t head = head_order_array[h];
				if(oms[head].can_play_freq(mc.current_notes[n].freq)){
					if(is_head_available_array[head]){
						//assign the note in the head note array
						is_head_available_array[head] = false;
						head_note_array[head] = mc.current_notes[n].note;
						head_channel_array[head] = mc.current_notes[n].channel;
						oms[head].play_freq(mc.current_notes[n].freq);
						//if note triggers are enabled, trigger an effect
						if(note_trigger_is_enabled){
							oms[head].animation->trigger_event(note_trigger_type[head]);
						}
						//move the head to the end of the pending head order:
						pending_head_order_to_end(head);
						#ifdef NOTE_DEBUG
							Serial.print("H");
							Serial.print(head);
							Serial.print(": Playing Note ");
							Serial.println(mc.current_notes[n].note);
						#endif
						//increase the number of assigned heads:
						num_assigned_heads++;
						//break the for loop for the heads once a head is assigned
						//otherwise they will all try to play the same note.
						break;
					} //end if head is available
					//in case no head could play the note, output debug
					if(h == 5){
						#ifdef NOTE_DEBUG
							Serial.print("No head for note: ");
							Serial.println(mc.current_notes[n].note);
						#endif
					}
				}//end if can play freq
			}//end head for loop
			//if all heads are assigned, break the note for loop:
			if(num_assigned_heads >= OM_NUM_OMIDITONES){
				break;
			}
		}//end note for loop
		//make sure sound is off for any heads not currently assigned:
		for(int h=0; h<OM_NUM_OMIDITONES; h++){
			if(is_head_available_array[h]){
				oms[h].sound_off();
			}
		}
	}//if note has changed or note was removed

	//update all heads every time
	for(int h=0; h<OM_NUM_OMIDITONES; h++){
		oms[h].update();
	}
}

//this function callc the lc.update() function whenever lighting is enabled, and marks the head tunings as invalid after
void update_lighting(void)
{
	if(lighting_is_enabled){
		int lighting_data_was_sent = lc.update();
		if(lighting_data_was_sent == LC_STRIP_WRITTEN){
			for(int h=0; h<OM_NUM_OMIDITONES; h++){
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
	for(int h=0; h<OM_NUM_OMIDITONES; h++){
		head_note_array[h] = MIDI_NO_NOTE;
		head_channel_array[h] = MIDI_NO_CHANNEL;
		head_order_array[h] = h;
		pending_head_order_array[h] = h;
		lc.add_animation(oms[h].animation);
		//init the om objects - This is going to take a while - like several minutes:
		oms[h].init();
		lc.update();
	}

	//initialize the MIDIController:
	mc.init();

	//assign MIDI CC handler functions as needed
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_9, handle_cc_9_hard_reset);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_14, handle_cc_14_pitch_correction_toggle);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_15, handle_cc_15_note_trigger_toggle);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_20, handle_cc_20_lighting_toggle);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_21, handle_cc_21_servo_toggle);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_22, handle_cc_22_om1_bg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_23, handle_cc_23_om2_bg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_24, handle_cc_24_om3_bg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_25, handle_cc_25_om4_bg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_26, handle_cc_26_om5_bg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_27, handle_cc_27_om6_bg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_28, handle_cc_28_om1_bg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_29, handle_cc_29_om2_bg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_30, handle_cc_30_om3_bg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_31, handle_cc_31_om4_bg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_41, handle_cc_41_om1_servo_pos);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_46, handle_cc_46_om5_bg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_47, handle_cc_47_om6_bg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_52, handle_cc_52_om1_fg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_53, handle_cc_53_om2_fg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_54, handle_cc_54_om3_fg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_55, handle_cc_55_om4_fg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_56, handle_cc_56_om5_fg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_57, handle_cc_57_om6_fg_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_58, handle_cc_58_om1_fg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_59, handle_cc_59_om2_fg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_60, handle_cc_60_om3_fg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_61, handle_cc_61_om4_fg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_62, handle_cc_62_om5_fg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_63, handle_cc_63_om6_fg_change);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_85, handle_cc_85_om2_servo_pos);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_86, handle_cc_86_om3_servo_pos);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_87, handle_cc_87_om4_servo_pos);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_89, handle_cc_89_om5_servo_pos);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_90, handle_cc_90_om6_servo_pos);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_102, handle_cc_102_om1_trigger_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_103, handle_cc_103_om2_trigger_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_104, handle_cc_104_om3_trigger_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_105, handle_cc_105_om4_trigger_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_106, handle_cc_106_om5_trigger_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_107, handle_cc_107_om6_trigger_rainbow);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_108, handle_cc_108_om1_trigger);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_109, handle_cc_109_om2_trigger);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_110, handle_cc_110_om3_trigger);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_111, handle_cc_111_om4_trigger);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_112, handle_cc_112_om5_trigger);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_113, handle_cc_113_om6_trigger);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_114, handle_cc_114_om1_note_trigger_type);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_115, handle_cc_115_om2_note_trigger_type);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_116, handle_cc_116_om3_note_trigger_type);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_117, handle_cc_117_om4_note_trigger_type);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_118, handle_cc_118_om5_note_trigger_type);
	mc.assign_MIDI_cc_handler(MIDI_CC::undefined_119, handle_cc_119_om6_note_trigger_type);

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