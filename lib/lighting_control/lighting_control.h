/*
This is a lighting control object built for WS2812 strips using the Adafruit 
Neopixel library.
It's mostly just a wrapper around a bunch of common lighting functions I've used 
in my projects in the past to make it more general and easier to insert into 
other projects.

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

#ifndef LIGHTING_CONTROL_H
#define LIGHTING_CONTROL_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <colors.h>

//comment this to disable debug messages about animations being added or removed to the lighting controller
#define ANIMATION_DEBUG

//comment this out to disable trigger event debug messages
//#define TRIGGER_DEBUG

//comment this to disable the VERY SPAMMY debug messages for rainbow fill
//#define RAINBOW_FILL_DEBUG

//comment this to disable the VERY SPAMMY debug messages for marquee fill
//#define MARQUEE_FILL_DEBUG

/*
Lighting Mode Overview:

Lighting modes have two parts stored in a single variable, a foreground and a background, totaling one 16-bit stored value.
If you want to have a solid background color with a marquee, you would set the lighting mode with a binary or of the two types, i.e.: (LC_BG_SOLID | LC_FG_MARQUEE_SOLID)
The least significant 8 bits represent the background, and the most significant 8 bits represent the foreground animation, and these are accounted for in the definitions below.
In addition to the foreground, special trigger events can cause additional effects such as color shots and color pulses.
These additional triggered events occur independent of any lighting modes, and override all background and foreground animations.

Note:
	All animations are entirely deterministic based on the current_frame/total_frames, or in the case of trigger events, the number of frames defined above for that event,
	i.e. LC_COLOR_PULSE_FADE_IN_FRAMES+LC_COLOR_PULSE_FADE_OUT_FRAMES. Each frame will be calculated on the fly, and will completely override any previous frames.
	The led_color_array values will be fully replaced every frame of every animation, to ensure that the current animations are always displayed correctly.

To summarize:
	-There are three types of effects, background, foreground, and triggered animations.
	-The background will render first,
	-then any foreground animation will render over the background overriding it as needed,
	-and finally any triggered animations will render over both.
*/

//this is the most animations that can be run at one time by the LightingControl object
//save on memory by setting it as low as you can get away with.
#define MAX_ANIMATIONS 6

//this is the most LEDs a lighting controller can manage
//usually best to set it to match your actual number of LEDs to save on memory usage.
#define MAX_NUM_LEDS 108

//this is the most trigger events any animation can have running at a time
//this will save a bit of memory if you reduce it, but not as much as the above two.
#define MAX_TRIGGER_EVENTS 20


//First make a couple masks so you can separate the FG from the BG later:
#define LC_BG_MASK 0x00FF
#define LC_FG_MASK 0xFF00

//definitions for various lighting modes:

//these are background modes, they are rendered first, and all others are rendered over them as specified below.

//this turns off all the leds in the animation. It does not change once drawn.
#define LC_BG_OFF 0x0001

//this shows a solid unchanging color on all the leds in the background. The color will be the first in the rainbow.
//you can step to other colors in the rainbow by external BG_TRIGGER, otherwise it does not change once drawn.
#define LC_BG_SOLID 0x0002

//this will slowly fade all the leds as a single color fading through the colors of a rainbow.
//color offset can be externally BG_TRIGGERed to the next color in the rainbow, or will move at a constant rate.
#define LC_BG_SLOW_FADE 0x0003

//this will populate a rainbow's colors evenly across the LED in the animation in order. It does not animate once drawn.
#define LC_BG_RAINBOW_FIXED 0x0004

//this will populate a rainbow like above, but it will animate it by offseting the color pattern over time.
#define LC_BG_RAINBOW_SLOW_ROTATE 0x0005


//These are foreground modes, they will render animations over the background modes above.

//this is a mode that has no additional foreground animation over the background animation.
#define LC_FG_NONE 0x0000

//this will display a fixed pattern the same as a marquee chase animation that will only move if the offset is changed manually where the color is always a solid constant color.
#define LC_FG_MARQUEE_SOLID_FIXED 0x0100

//this will display a single-color marquee style pixel chase animation.
#define LC_FG_MARQUEE_SOLID 0x0200

//this will display a fixed pattern the same as a marquee chase animation that will only move if the offset is changed manually where the color of all the LEDs slowly fades through the colors of a rainbow.
#define LC_FG_MARQUEE_SLOW_FADE_FIXED 0x0300

//this will display a marquee style animation where the color of all the LEDs slowly fades through the colors of a rainbow.
#define LC_FG_MARQUEE_SLOW_FADE 0x0400

//this will render the foreground rainbow based on the offset value, and leave LEDs below the offset value alone.
#define LC_FG_VU_METER 0x0500


//these are trigger_event animations. They operate only when called externally by trigger_event(), and override the foreground and background effects.

//this will trigger a change in the background lighting, depending on the mode:
#define LC_TRIGGER_BG 0

//this will trigger a change in the foreground lighting, depending on the mode:
#define LC_TRIGGER_FG 1

//this will cause a pulse of a single color to appear somewhere randomly along the led array. It will fade in, then fade back out one time per trigger.
//pulses need to be triggered externally.
//fade in and out times can be adjusted separately.
#define LC_TRIGGER_COLOR_PULSE 2

//this will cause a pulse of to appear somewhere randomly along the led array. It will fade in, then fade back out one time per trigger.
//All pulses will be the same color, and the color will change over time.
//color offset can be set externally, or to a steady rate
//pulse need to be triggered externally.
//fade in and out times can be adjusted separately.
#define LC_TRIGGER_COLOR_PULSE_SLOW_FADE 3

//this will cause a pulse of to appear somewhere randomly along the led array. It will fade in, then fade back out one time per trigger.
//Each pulse will be a new color in the order of the rainbow.
//pulse need to be triggered externally.
//fade in and out times can be adjusted separately.
#define LC_TRIGGER_COLOR_PULSE_RAINBOW 4

//this will fire off colored pulses of a single color down the LED strip when triggered externally.
//shots need to be externally triggered.
#define LC_TRIGGER_COLOR_SHOT 5

//this will fire off color pulses that slowly fade along the colors of a rainbow over time.
//color offset can be changed externally, or set to a constant rate
//shots need to be externally triggered.
#define LC_TRIGGER_COLOR_SHOT_SLOW_FADE 6

//this will fire off color pulses with a new color for each pulse, in the order of the colors of a rainbow.
//shots need to be externally triggered.
#define LC_TRIGGER_COLOR_SHOT_RAINBOW 7

//this will flash all the LEDs for the head a single color for a short time.
//needs to be externallt triggered
#define LC_TRIGGER_FLASH 8

//this will flash all the LEDs for the head a single color for a short time.
//color offset can be changed externally, or set to a constant rate
//needs to be externallt triggered
#define LC_TRIGGER_FLASH_SLOW_FADE 9

//this will flash all the LEDs for the head a single color for a short time.
//Each flash will be a new color in the order of the rainbow.
//needs to be externallt triggered
#define LC_TRIGGER_FLASH_RAINBOW 10

//defines to make the code more readable:
#define POSITIVE 1
#define STOPPED 0
#define NEGATIVE -1

#define LEFT 0
#define RIGHT 1

#define NOT_IN_ARRAY -1

#define LC_BG 1
#define LC_FG 2
#define LC_TRIGGER 3

#define LC_STRIP_NOT_WRITTEN 0
#define LC_STRIP_WRITTEN 1

//some default values for various animation modes:

//this is the default refresh rate for the animations in ms between frames. 33 ~= 30Hz, 22 ~= 45Hz, 16 ~= 60Hz.
#define LC_DEFAULT_REFRESH_RATE 33

//Variables for the number of frames per animation:
//this is how many frames to go from one color to the next, not through the whole rainbow.
#define LC_SLOW_FADE_FRAMES 100

//this is how many frames for the entire cycle to repeat, i.e. the first dot returns to the origin
#define LC_MARQUEE_FRAMES 150

//this is how many frames to rotate fully through the entire led_array and get back to the beginning position.
#define LC_RAINBOW_SLOW_ROTATE_FRAMES 300

//this is how many frames to fade colors on a trigger event using slow fade
#define LC_TRIGGER_SLOW_FADE_FRAMES 100

//this is how long for one color shot to go from its origin to the end of the strip.
#define LC_COLOR_SHOT_FRAMES 15

//this is how many frames each color pulse fades to its maximum brightness.
#define LC_COLOR_PULSE_FADE_IN_FRAMES 5
//this is how many frames for a color pulse to fade back out to black.
#define LC_COLOR_PULSE_FADE_OUT_FRAMES 20

//this is how many frames each flash will fade to its maximum brightness.
#define LC_FLASH_FADE_IN_FRAMES 1
//this is how many frames for a flash to fade out to black.
#define LC_FLASH_FADE_OUT_FRAMES 10

//a couple defines for marquee proportions generation
//these look better if (LC_MARQUEE_ON+LC_MARQUEE_OFF)*LC_MARQUEE_SUBDIVISIONS is the same as the number of your LEDs.
//matching the number of LEDs is not required, but if you can line it up, your animations are less likely to have weird spacing due to misalignment issues with LED spacing.
#define LC_MARQUEE_ON 1
#define LC_MARQUEE_OFF 1
#define LC_MARQUEE_SUBDIVISIONS 9

//this is the overflow value for the offsets, if an offset is larger than LC_MAX_OFFSET or less than negative LC_MAX_OFFSET, it will be adjusted to be within spec.
//this is also the most discreet positions that any rotating animation such as the LC_BG_RAINBOWs can have.
//animation timing is determined by the total frame numbers above multiplied by the refresh rate, but no animation will have more discreet positions than this number
#define LC_MAX_OFFSET 36000
//this is the default offset for animations in degrees. If you need your animation to start in a different location along the led_positions array, adjust this from 0.
#define LC_DEFAULT_OFFSET 0
//Default position for shot origin in 1/100ths of a degree. They will always count down to 0/36000 (the first LED) from the shot position.
#define LC_DEFAULT_SHOT_POSITION 18000

//a color correction table for the neopixel color settings:
const uint8_t PROGMEM gamma8[] = {
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
	1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
	2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
	5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
	10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
	115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
	177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
	215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

//this is a variable for avoiding setting colors to be off based on the start of this table being mostly off.
//It's still pretty rough near the low end with 8-bit colors. Not much I can do about it currently.
#define FIRST_NON_OFF_COLOR 28

//this is a structure for holding trigger_event animations. It will track the frames, color, and position or any trigger_events that aren't LC_TRIGGER_BG or LC_TRIGGER_FG events.
struct TriggerEvent{
	uint16_t type;
	int32_t total_frames;
	int32_t current_frame;
	uint32_t color;
	uint32_t offset;
	elapsedMillis last_update;
	bool event_has_completed;
	bool direction;
};

//this is a class of object that will generate and keep track of animations on an array of LEDs.
//each animation object can be added to or removed from a lighting controller, which will then run that animation as specified.
//note that updating the animation object manually will only update the color values within the animation object's led_color_array.
//the animation will only display when it is assigned to be active on a LightingControl object.
class Animation{
	public:
		//constructor function - this sets defaults based on lighting_mode when the object is created, and then further modification is possible with additional function calls.
		//the led_array and num_leds_in_array will not be changeabe, however.
		Animation(uint16_t * led_array, uint16_t num_leds_in_array, uint16_t lighting_mode, rainbow bg_colors, rainbow fg_colors, rainbow trigger_event_colors, uint32_t update_interval = LC_DEFAULT_REFRESH_RATE);

		//this should be called after updating any settings (i.e. using any public functions other than update() and trigger_event())
		//It will generate initial colors for the led_color_array based on the current settings
		void init();

		//this should be called by the LightingControl object automatically to run the animations. Doing it manually won't guarantee the animation is displayed.
		void update();

		//this can be called by programs to trigger events, such as LM_SHOTs or LM_COLOR_PULSES - what happens depends on the lighting_mode
		void trigger_event(uint16_t trigger_type);

		//this will change the foreground or background lighting mode, resetting all values to their defaults for that lighting mode:
		void change_lighting_mode(uint16_t new_lighting_mode);

		//this will change the animation to use another rainbow of the specified type. All updates performed after this will use the new rainbow colors starting with the next frame.
		//the type can be LC_BG, LC_FG, or LC_TRIGGER, to set which rainbow to change.
		void change_rainbow(uint8_t type, rainbow new_rainbow, uint16_t new_rainbow_position = 0);

		//this will change the animation's offsets. This can be used for manual movement of any offsettable animation, such as the LC_BG_RAINBOW_FIXED, or LC_FG_MARQUEE_SOLID_FIXED
		//the type can be LC_BG, LC_FG, or LC_TRIGGER, to set which offset to change.
		void change_offset(uint8_t type, int32_t new_offset, int32_t alternate_max_value = LC_MAX_OFFSET);

		//these return the current fg and bg animation modes:
		uint16_t current_bg_mode();
		uint16_t current_fg_mode();

		//this stores the leds the animation works on, in sequential order.
		//The values in this array are the position of the Adafruit_NeoPixel strip object, in the order to animate on.
		//Make sure the led array you provide is in the right order, or animations will not work as expected.
		uint16_t * led_positions;

		//this array holds the current colors that correspond the the positions in the led_positions array.
		//These are not 'live' unless the animation is set as active on a LightingControl object
		uint32_t * led_color_array;

		//this is how many LEDs are in the leds array to be animated across.
		uint16_t num_leds;

	private:
		//this will update the trigger animations in the active_triggers array and render the effected LEDs.
		//Note the more recent trigger events will override older if there is a conflict.
		void update_trigger_animations();

		//this will add a new trigger animation to the active_triggers[] array.
		void add_trigger_event(TriggerEvent event);

		//this will remove a new trigger animation from the active_triggers[] array.
		void clean_trigger_events();

		//this will fill an LED array with a solid color.
		void fill_solid(uint32_t * led_array, uint16_t num_leds, uint32_t color);

		//this will populate an LED array with a rainbow with an origin based on an offset value,
		//and an additional offset based on the number of frames out of a total number of frames.
		void fill_rainbow(uint32_t * led_array, uint16_t num_leds, rainbow rainbow, int32_t origin_offset = LC_DEFAULT_OFFSET, uint32_t current_frame = 0, uint32_t total_frames = 0);

		//this will populate an LED array with a marquee with an origin based on an offset value,
		//and an additional offset based on the number of frames out of a total number of frames.
		void fill_marquee(uint32_t * led_array, uint16_t num_leds, uint32_t color, int32_t origin_offset = LC_DEFAULT_OFFSET, uint32_t current_frame = 0, uint32_t total_frames = 0);

		//this will populate a VU-meter style animation that is entirely dependent on setting the external offset value.
		//it wil render the rainbow up to the offset, and leave the background running above the offset
		void fill_vu_meter(uint32_t * led_array, uint16_t num_leds, rainbow rainbow, int32_t origin_offset = LC_DEFAULT_OFFSET);

		//this will return an adjusted color based on the current/total frame ratio between two colors of a rainbow.
		uint32_t slow_fade_color_adjust(rainbow rainbow, uint8_t current_color_position, uint32_t current_frame, uint32_t total_frames);

		//this will increment the current_frame until total_frames and then reset it to 0.
		void increment_frame();

		//these will increment the current_rainbow_color of the appropriate rainbow until overflow and then return to color 0
		void increment_bg_rainbow();
		void increment_fg_rainbow();
		void increment_trigger_rainbow();

		//this will adjust the offset number so that it is always within bounds
		uint32_t correct_offset(int32_t origin_offset);

		//this notes the animation types for the object.
		//It can be changed at a later time, and will reset all settings to defaults upon a lighting mode change.
		uint16_t bg_mode;
		uint16_t fg_mode;

		//these are the rainbows (array of colors, in order) that the animation will use
		rainbow bg_rainbow;
		rainbow fg_rainbow;
		rainbow trigger_rainbow;

		//these are the current rainbow color for the three modes:
		uint16_t current_bg_rainbow_color;
		uint16_t current_fg_rainbow_color;
		uint16_t current_trigger_rainbow_color;

		//variables to track timing:
		//this is reset every time a new frame is rendered and stored
		elapsedMicros last_animation_step;
		//this is the minimum time before another update can be triggered
		uint32_t min_time_between_updates;

		//these track when a foreground or background trigger event has occurred. The action will be taken at the next update.
		//other trigger events will be handled by the trigger_event() function depending on the type of event.
		bool bg_event_has_occurred;
		bool fg_event_has_occurred;

		//these are variables for tracking colors and frame positions and offsets.
		//these are the number of frames for a full cycle of a lighting mode. How it's used will depend heavily on the mode.
		uint32_t total_bg_frames;
		uint32_t total_fg_frames;
		uint32_t total_marquee_fade_frames;
		uint32_t total_trigger_fade_frames;
		//these are the current frames of the animations.
		uint32_t current_bg_frame;
		uint32_t current_fg_frame;
		uint32_t current_marquee_fade_frame;
		uint32_t current_trigger_fade_frame;

		//These are offset values given in 1/100ths of a degree (e.g. 36000 is a full cycly, 18000 is halfway, etc.).
		//this is an offset value used for adjusting the foreground position, typically in degrees.
		uint32_t bg_offset;
		//this is an offset value used for adjusting the background position, typically in degrees.
		uint32_t fg_offset;
		//this is an offset value used for adjusting the triggered event origin position, typically in degrees.
		uint32_t trigger_offset;

		//these next few variables are only used for marquee animations:
		//the number of 'dots' to have over all
		uint16_t marquee_subdivisions;
		//these are the proportion of 'on' space to 'off' space for each subdivision above.
		uint16_t marquee_on_subpips;
		uint16_t marquee_off_subpips;
		//this is the total number of subpips in the marquee.
		uint16_t total_marquee_subpips;

		//the next couple variables are for color pulses:
		//these are the fade_in and fade_out times for color pulses
		uint16_t color_pulse_fade_in;
		uint16_t color_pulse_fade_out;

		//this will be an array of trigger_event structures that control the animations caused by trigger_events.
		TriggerEvent active_triggers[MAX_TRIGGER_EVENTS];

		//this tracks how many trigger events are currently in the active_triggers array.
		uint16_t num_trigger_events;

		//this tracks the last direction a shot event was fired during an animation so it can send the next one in the other direction.
		bool last_color_shot_direction;
};

//this will run animations on an Adafruit_NeoPixel LED strip object. The animations are defined above in the animation class.
class LightingControl{
	public:
		//constructor function
		LightingControl(Adafruit_NeoPixel *LED_controller, uint8_t max_brightness);

		//this should be called during setup to initialize and blank the Adafruit_NeoPixel led strip prior to doing anything else.
		void init();

		//call this in your main program loop to keep all animations running. Animations will not run without this
		int update();

		//this adds an animation to the controller, which will then be updated with all the lighting effects on the LightingControl object
		void add_animation(Animation * new_animation);

		//this removes an animation from the controller, and stops it from being updated by the LightingControl object
		void rm_animation(Animation * animation_to_be_removed);

	private:
		//this is a wrapper function to send corrected color values to the controller.
		//It is recommended to use this to send animation updates instead of sending them directly using strip.SetPixelColor().
		void set_corrected_color(uint16_t led, uint32_t color);

		//this will check the currently_running_animations array to see if an animation object is already present:
		int16_t check_animations(Animation * animation);

		//pointer to the neopixel strip being controller by this LightingControl object:
		Adafruit_NeoPixel * strip;

		//this is the max brightness for the Adafruit_NeoPixel object LEDs. It gets set only once during init. Use Animation objects for fading.
		uint8_t brightness;

		//array of pointers to currently running animation objects on the LightingControl object
		Animation * currently_running_animations[MAX_ANIMATIONS];

		//this tracks how many total animations are on the lighting controller
		uint16_t num_current_animations;

		//this is for keeping updates to a reasonable amount on the animation objects.
		elapsedMillis last_update;
};

#endif