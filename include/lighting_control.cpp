/*
I'll copy over the comments when it's finished
*/

#include "lighting_control.h"

/* ----- BEGIN ANIMATION CLASS FUNCTIONS -----*/

//constructor function - assigns default values based on lighting mode:
Animation::Animation(uint16_t * led_array, uint16_t num_leds_in_array, uint16_t lighting_mode, rainbow bg_colors, rainbow fg_colors, rainbow trigger_event_colors, uint32_t update_interval){
	//set values based on the init inputs first:
	led_positions = led_array;
	num_leds = num_leds_in_array;
	bg_mode = lighting_mode & LC_BG_MASK;
	fg_mode = lighting_mode & LC_FG_MASK;
	bg_rainbow = bg_colors;
	fg_rainbow = fg_colors;
	trigger_rainbow = trigger_event_colors;
	min_time_between_updates = update_interval;
	//init the color array to be the size of the led_positions[] array.
	led_color_array = new uint32_t[num_leds];
	for(int i=0; i<num_leds; i++){
		led_color_array[i] = off;
	}

	//call the init function for generation of the object for mode-specific initialization
	init();
}

/* ----- PUBLIC FUNCTIONS BELOW ----- */

//this should be called after updating the lighting mode. It will reset everything to the defaults based on the lighting mode changes.
//If you want to switch to specific rainbow colors, offsets, etc., do it after the init.
//Init will generate initial colors for the led_color_array based on the current settings.
//It will also always reset the last_animation_step time for immediate changes to be shown.
void Animation::init(){
	//set defaults for all variables, and adjust only the ones that need to be adjusted in the if statements below:
	current_bg_rainbow_color = 0;
	current_fg_rainbow_color = 0;
	current_trigger_rainbow_color = 0;
	bg_event_has_occurred = false;
	fg_event_has_occurred = false;
	total_bg_frames = 0; //this will be set to actual values below, 0 is default only for solid animations
	total_fg_frames = 0; //this will be set to actual values below, 0 is default only for solid animations
	total_marquee_fade_frames = 0; //this will be set to actual values below, 0 is default only for solid animations
	//the trigger event slow fade frames should be set regardless, as they operate independently of setting off any triggers:
	total_trigger_fade_frames = LC_TRIGGER_SLOW_FADE_FRAMES;
	current_bg_frame = 0;
	current_fg_frame = 0;
	current_marquee_fade_frame = 0;
	current_trigger_fade_frame = 0;
	last_color_shot_direction = LEFT;
	//note trigger_events will be on a struct that tracks their frames separately from the rest of the animations
	fg_offset = correct_offset(LC_DEFAULT_OFFSET);
	bg_offset = correct_offset(LC_DEFAULT_OFFSET);
	trigger_offset = correct_offset(LC_DEFAULT_OFFSET);
	marquee_subdivisions = LC_MARQUEE_SUBDIVISIONS;
	marquee_on_subpips = LC_MARQUEE_ON;
	marquee_off_subpips = LC_MARQUEE_OFF;
	color_pulse_fade_in = LC_COLOR_PULSE_FADE_IN_FRAMES;
	color_pulse_fade_out = LC_COLOR_PULSE_FADE_OUT_FRAMES;
	total_marquee_subpips = marquee_subdivisions*(marquee_on_subpips+marquee_off_subpips);

	//then set defaults based on the modes:
	if(bg_mode == LC_BG_OFF){
		//set all led colors to be off:
		fill_solid(led_color_array, num_leds, off);
	} else if(bg_mode == LC_BG_SOLID){
		//don't need to change any of the defaults above.
		//set all leds to the current_bg_rainbow_color:
		fill_solid(led_color_array, num_leds, bg_rainbow.colors[current_bg_rainbow_color]);
	} else if(bg_mode == LC_BG_SLOW_FADE){
		total_bg_frames = LC_SLOW_FADE_FRAMES;
		//set color to initial rainbow color with no offset, as this is the first frame:
		fill_solid(led_color_array, num_leds, bg_rainbow.colors[current_bg_rainbow_color]);
	} else if(bg_mode == LC_BG_RAINBOW_FIXED){
		//don't need to change any of the defaults above.
		//generate a rainbow based on the offset and ignore frames (the last two arguments default to 0):
		fill_rainbow(led_color_array, num_leds, bg_rainbow, bg_offset);
	} else if(bg_mode == LC_BG_RAINBOW_SLOW_ROTATE){
		total_bg_frames = LC_RAINBOW_SLOW_ROTATE_FRAMES;
		//generate a rainbow baed on the offset value, no need to adjust based on frames as this is the first frame:
		fill_rainbow(led_color_array, num_leds, bg_rainbow, bg_offset, current_bg_frame, total_bg_frames);
	}

	if(fg_mode == LC_FG_MARQUEE_SOLID_FIXED){
		//generate a marquee over the bg based on the offset value, no need to adjust based on frames as this is the FIXED varsion that only moved per externally set offset:
		fill_marquee(led_color_array, num_leds, fg_rainbow.colors[current_fg_rainbow_color], fg_offset);
	} else if(fg_mode == LC_FG_MARQUEE_SOLID){
		total_fg_frames = LC_MARQUEE_FRAMES;
		//generate a marquee over the bg based on the offset value, no need to adjust based on frames as this is the first frame:
		fill_marquee(led_color_array, num_leds, fg_rainbow.colors[current_fg_rainbow_color], fg_offset, current_fg_frame, total_fg_frames);
	} else if(fg_mode == LC_FG_MARQUEE_SLOW_FADE_FIXED){
		total_marquee_fade_frames = LC_SLOW_FADE_FRAMES;
		//generate a marquee over the bg based on the offset value, no need to adjust based on frames as this is fixed and only updates per externally set offset:
		fill_marquee(led_color_array, num_leds, fg_rainbow.colors[current_fg_rainbow_color], fg_offset);
	} else if(fg_mode == LC_FG_MARQUEE_SLOW_FADE){
		total_fg_frames = LC_MARQUEE_FRAMES;
		total_marquee_fade_frames = LC_SLOW_FADE_FRAMES;
		//generate a marquee over the bg based on the offset value, no need to adjust based on frames as this is the first frame:
		fill_marquee(led_color_array, num_leds, fg_rainbow.colors[current_fg_rainbow_color], fg_offset, current_fg_frame, total_fg_frames);
	} else if(fg_mode == LC_FG_VU_METER){
		//render the VU meter based on the offset value
		fill_vu_meter(led_color_array, num_leds, fg_rainbow, fg_offset);
	}

	//increment by one frame after so the next update won't repeat the first frame:
	increment_frame();

	//always reset the animation step time on an init regardless of how much time since the last update:
	last_animation_step = 0;
}

//this should be called by the LightingControl object automatically to run the animations. Doing it manually won't guarantee the animation is displayed.
void Animation::update(){
	if(last_animation_step > min_time_between_updates){
		//update background first.
		if(bg_mode == LC_BG_OFF){
				//set all led colors to be off:
				fill_solid(led_color_array, num_leds, off);
		} else if(bg_mode == LC_BG_SOLID){
				//set all leds to the current_bg_rainbow_color:
				fill_solid(led_color_array, num_leds, bg_rainbow.colors[current_bg_rainbow_color]);
		} else if(bg_mode == LC_BG_SLOW_FADE){
				//set color to adjusted color based on current/total frames:
				uint32_t adjusted_color = slow_fade_color_adjust(bg_rainbow, current_bg_rainbow_color, current_bg_frame, total_bg_frames);
				fill_solid(led_color_array, num_leds, adjusted_color);
		} else if(bg_mode == LC_BG_RAINBOW_FIXED){
				//generate a rainbow based on the offset and ignore frames (the last two arguments default to 0 and 0):
				fill_rainbow(led_color_array, num_leds, bg_rainbow, bg_offset);
		} else if(bg_mode == LC_BG_RAINBOW_SLOW_ROTATE){
				//generate a rainbow baed on the offset value and current/total frames:
				fill_rainbow(led_color_array, num_leds, bg_rainbow, bg_offset, current_bg_frame, total_bg_frames);
		}

		//update foreground animations over the background colors now stored.
		if(fg_mode == LC_FG_MARQUEE_SOLID_FIXED){
			total_fg_frames = LC_MARQUEE_FRAMES;
			//generate a marquee over the bg based on the offset value, no need to adjust based on frames as this is the FIXED varsion that only moved per externally set offset:
			fill_marquee(led_color_array, num_leds, fg_rainbow.colors[current_fg_rainbow_color], fg_offset);
		} else if(fg_mode == LC_FG_MARQUEE_SOLID){
			//generate a marquee over the bg based on the offset value and current/total frames:
			fill_marquee(led_color_array, num_leds, fg_rainbow.colors[current_fg_rainbow_color], fg_offset, current_fg_frame, total_fg_frames);
		} else if(fg_mode == LC_FG_MARQUEE_SLOW_FADE_FIXED){
			//adjust the color. Note the adjusted color is being adjusted based on the marquee_fade_frames, not the fg_frames,
			//so you can (and probably should)set both of these values separately.
			uint32_t adjusted_color = slow_fade_color_adjust(fg_rainbow, current_fg_rainbow_color, current_marquee_fade_frame, total_marquee_fade_frames);
			//generate a marquee over the bg based on the offset value, no need to adjust based on frames as this is fixed and only updates per externally set offset:
			fill_marquee(led_color_array, num_leds, adjusted_color, fg_offset);
		} else if(fg_mode == LC_FG_MARQUEE_SLOW_FADE){
			//adjust the color. Note the adjusted color is being adjusted based on the marquee_fade_frames, not the fg_frames,
			//so you can (and probably should)set both of these values separately.
			uint32_t adjusted_color = slow_fade_color_adjust(fg_rainbow, current_fg_rainbow_color, current_marquee_fade_frame, total_marquee_fade_frames);
			//generate a marquee over the bg based on the offset value and current/total frames:
			fill_marquee(led_color_array, num_leds, adjusted_color, fg_offset, current_fg_frame, total_fg_frames);
		} else if(fg_mode == LC_FG_VU_METER){
			//render the VU meter based on the offset value
			fill_vu_meter(led_color_array, num_leds, fg_rainbow, fg_offset);
		}

		//update any trigger animations currently in the buffer
		update_trigger_animations();

		//increment by one frame after each update:
		increment_frame();
		//reset the timer after each update
		last_animation_step = 0;
	} //timing check if statement
}

//this can be called by programs to trigger events, such as LM_SHOTs or LM_COLOR_PULSES - what happens depends on the lighting_mode
void Animation::trigger_event(uint16_t trigger_type){
	//this can only happen once per frame, so it doesn't matter if you call it once or 200 times during a frame.
	if(trigger_type == LC_TRIGGER_BG){
		bg_event_has_occurred = true;
	} else if(trigger_type == LC_TRIGGER_FG){
		fg_event_has_occurred = true;
	} else if(trigger_type == LC_TRIGGER_COLOR_PULSE){
		//make a new solid color pulse of the current trigger rainbow color and add it to the active_triggers array.
		TriggerEvent new_pulse = {
			.type = trigger_type,
			.total_frames = LC_COLOR_PULSE_FADE_IN_FRAMES + LC_COLOR_PULSE_FADE_OUT_FRAMES,
			.current_frame = 0,
			.color = trigger_rainbow.colors[current_trigger_rainbow_color],
			.offset = random(LC_MAX_OFFSET),
			.last_update = 0,
			.event_has_completed = false,
			.direction = LEFT
		};
		add_trigger_event(new_pulse);
	} else if(trigger_type == LC_TRIGGER_COLOR_PULSE_SLOW_FADE){
		//make a new color pulse of the offset current trigger_rainbow color based on current_frame and add it to the active_triggers array.
		uint32_t adjusted_color = slow_fade_color_adjust(trigger_rainbow, current_trigger_rainbow_color, current_trigger_fade_frame, total_trigger_fade_frames);
		TriggerEvent new_pulse = {
			.type = trigger_type,
			.total_frames = LC_COLOR_PULSE_FADE_IN_FRAMES + LC_COLOR_PULSE_FADE_OUT_FRAMES,
			.current_frame = 0,
			.color = adjusted_color,
			.offset = random(LC_MAX_OFFSET),
			.last_update = 0,
			.event_has_completed = false,
			.direction = LEFT
		};
		add_trigger_event(new_pulse);
	} else if(trigger_type == LC_TRIGGER_COLOR_PULSE_RAINBOW){
		//make a new color pulse of the current trigger_rainbow color and add it to the active_triggers array and increment the trigger_rainbow color.
		TriggerEvent new_pulse = {
			.type = trigger_type,
			.total_frames = LC_COLOR_PULSE_FADE_IN_FRAMES + LC_COLOR_PULSE_FADE_OUT_FRAMES,
			.current_frame = 0,
			.color = trigger_rainbow.colors[current_trigger_rainbow_color],
			.offset = random(LC_MAX_OFFSET),
			.last_update = 0,
			.event_has_completed = false,
			.direction = LEFT
		};
		add_trigger_event(new_pulse);
		increment_trigger_rainbow();
	} else if(trigger_type == LC_TRIGGER_COLOR_SHOT){
		//make a new solid color shot of the current trigger rainbow color and add it to the active_triggers array.
		last_color_shot_direction = !last_color_shot_direction;
		TriggerEvent new_shot = {
			.type = trigger_type,
			.total_frames = LC_COLOR_SHOT_FRAMES,
			.current_frame = 0,
			.color = trigger_rainbow.colors[current_trigger_rainbow_color],
			.offset = LC_DEFAULT_SHOT_POSITION,
			.last_update = 0,
			.event_has_completed = false,
			.direction = last_color_shot_direction
		};
		add_trigger_event(new_shot);
	} else if(trigger_type == LC_TRIGGER_COLOR_SHOT_SLOW_FADE){
		//make a new color shot of the offset current trigger_rainbow color based on current_frame and add it to the active_triggers array.
		uint32_t adjusted_color = slow_fade_color_adjust(trigger_rainbow, current_trigger_rainbow_color, current_trigger_fade_frame, total_trigger_fade_frames);
		last_color_shot_direction = !last_color_shot_direction;
		TriggerEvent new_shot = {
			.type = trigger_type,
			.total_frames = LC_COLOR_SHOT_FRAMES,
			.current_frame = 0,
			.color = adjusted_color,
			.offset = LC_DEFAULT_SHOT_POSITION,
			.last_update = 0,
			.event_has_completed = false,
			.direction = last_color_shot_direction
		};
		add_trigger_event(new_shot);
	} else if(trigger_type == LC_TRIGGER_COLOR_SHOT_RAINBOW){
		//make a new color shot of the current trigger_rainbow color and add it to the active_triggers array and increment the trigger_rainbow color.
		increment_trigger_rainbow();
		//change direction
		last_color_shot_direction = !last_color_shot_direction;
		TriggerEvent new_shot = {
			.type = trigger_type,
			.total_frames = LC_COLOR_SHOT_FRAMES,
			.current_frame = 0,
			.color = trigger_rainbow.colors[current_trigger_rainbow_color],
			.offset = LC_DEFAULT_SHOT_POSITION,
			.last_update = 0,
			.event_has_completed = false,
			.direction = last_color_shot_direction
		};
		add_trigger_event(new_shot);
		increment_trigger_rainbow();
	} else if(trigger_type == LC_TRIGGER_FLASH){
		//make a new solid color flash the current trigger rainbow color and add it to the active_triggers array.
		TriggerEvent new_flash = {
			.type = trigger_type,
			.total_frames = LC_FLASH_FADE_IN_FRAMES + LC_FLASH_FADE_OUT_FRAMES,
			.current_frame = 0,
			.color = trigger_rainbow.colors[current_trigger_rainbow_color],
			.offset = 0,
			.last_update = 0, /* not needed for flashes */
			.event_has_completed = false,
			.direction = LEFT
		};
		add_trigger_event(new_flash);
	} else if(trigger_type == LC_TRIGGER_FLASH_SLOW_FADE){
		//make a new flash of the offset current trigger_rainbow color based on current_frame and add it to the active_triggers array.
		uint32_t adjusted_color = slow_fade_color_adjust(trigger_rainbow, current_trigger_rainbow_color, current_trigger_fade_frame, total_trigger_fade_frames);
		TriggerEvent new_flash = {
			.type = trigger_type,
			.total_frames = LC_FLASH_FADE_IN_FRAMES + LC_FLASH_FADE_OUT_FRAMES,
			.current_frame = 0,
			.color = adjusted_color,
			.offset = 0, /* not needed for flashes */
			.last_update = 0,
			.event_has_completed = false,
			.direction = LEFT
		};
		add_trigger_event(new_flash);
	} else if(trigger_type == LC_TRIGGER_FLASH_RAINBOW){
		//make a new flash of the current trigger_rainbow color and add it to the active_triggers array and increment the trigger_rainbow color.
		TriggerEvent new_flash = {
			.type = trigger_type,
			.total_frames = LC_FLASH_FADE_IN_FRAMES + LC_FLASH_FADE_OUT_FRAMES,
			.current_frame = 0,
			.color = trigger_rainbow.colors[current_trigger_rainbow_color],
			.offset = 0, /* not needed for flashes */
			.last_update = 0,
			.event_has_completed = false,
			.direction = LEFT
		};
		add_trigger_event(new_flash);
		increment_trigger_rainbow();
	}
}

//this will change the lighting mode, triggering an init(), and resetting all values to their defaults for that lighting mode:
void Animation::change_lighting_mode(uint16_t new_lighting_mode){
	bg_mode = new_lighting_mode & LC_BG_MASK;
	fg_mode = new_lighting_mode & LC_FG_MASK;
	init();
}

//these will change the animation to use another rainbow of the specified type. All updates performed after this will use the new rainbow colors from the next frame.
void Animation::change_rainbow(uint8_t type, rainbow new_rainbow, uint16_t new_rainbow_position){
	if(type == LC_BG){
		bg_rainbow = new_rainbow;
		current_bg_rainbow_color = new_rainbow_position;
	} else if(type ==  LC_FG){
		fg_rainbow = new_rainbow;
		current_fg_rainbow_color = new_rainbow_position;
	} else if(type ==  LC_TRIGGER){
		trigger_rainbow = new_rainbow;
		current_trigger_rainbow_color = new_rainbow_position;
	}
}

//this will change the animation's offsets. This can be used for manual movement of any offsettable animation, such as the LC_BG_RAINBOW_FIXED, or LC_FG_MARQUEE_SOLID_FIXED
void Animation::change_offset(uint8_t type, int32_t new_offset, int32_t alternate_max_value){
	//if an alternate_max_value is used, map the new_offset relative to the alternate_max_value onto the typical range from 0 to LC_MAX_OFFSET.
	//the alternate_max_value lets you use, for example, the maximum analog_read value of 1024 to set the offset instead of needing to convert it beforehand to be a #/36000.
	//this can be handy for things like the LC_FG_VU_METER animation, to directly map values based on a volume reading.
	if(alternate_max_value != LC_MAX_OFFSET){
		new_offset = map(new_offset, 0, alternate_max_value, 0, LC_MAX_OFFSET);
	}
	if(type == LC_BG){
		bg_offset = correct_offset(new_offset);
	} else if(type == LC_FG){
		fg_offset = correct_offset(new_offset);
	} else if(type == LC_TRIGGER){
		trigger_offset = correct_offset(new_offset);
	}
}

//these return the current fg and bg animation modes:
uint16_t Animation::current_bg_mode(){
	return bg_mode;
}
uint16_t Animation::current_fg_mode(){
	return fg_mode;
}

/* ----- END PUBLIC FUNCTIONS ----- */
/* ----- PRIVATE FUNCTIONS BELOW ----- */

//this will update the trigger animations in the active_triggers array and render the effected LEDs.
//Note the more recent trigger events will override older if they are on the same LEDs.
void Animation::update_trigger_animations(){
	//update bg_event based on modes:
	if(bg_event_has_occurred){
		//only update certain modes:
		if( (bg_mode == LC_BG_SOLID) ||
				(bg_mode == LC_BG_SLOW_FADE) ) {
			//increment the rainbow to the next color and reset the current_bg_frame to 0.
			increment_bg_rainbow();
			current_bg_frame = 0;
		}
		bg_event_has_occurred = false;
	}

	//update bg_event based on modes:
	if(fg_event_has_occurred){
		if( (fg_mode == LC_FG_MARQUEE_SOLID_FIXED) ||
			(fg_mode == LC_FG_MARQUEE_SOLID) ||
			(fg_mode == LC_FG_MARQUEE_SLOW_FADE_FIXED) ||
			(fg_mode == LC_FG_MARQUEE_SLOW_FADE) ){
			//increment the rainbow to the next color and reset the current_marquee_fade_frame to 0.
			increment_fg_rainbow();
			current_marquee_fade_frame = 0;
		}
		fg_event_has_occurred = false;
	}

	//update all events in active_triggers array:
	if(num_trigger_events > 0){
		for(int i=0; i<num_trigger_events; i++){
			//do the update based on the type of trigger event.
			//once created, the color will not change anymore, so the animations can all be carried out together for each type
			if(	active_triggers[i].type == LC_TRIGGER_COLOR_PULSE ||
				active_triggers[i].type == LC_TRIGGER_COLOR_PULSE_SLOW_FADE ||
				active_triggers[i].type == LC_TRIGGER_COLOR_PULSE_RAINBOW ){
				//animate a color pulse at the correct offset based on the current_frames for the event.

				//We can make an array that notes the offset position of each LED:
				//note that it's double the actual number of LEDs to make iteration a single for loop regardless of offset, so it can make it around the circle twice.
				uint32_t led_offsets[num_leds*2];
				led_offsets[0] = 0;
				for(int i=1; i<num_leds*2; i++){
					led_offsets[i] = led_offsets[i-1]+(LC_MAX_OFFSET / num_leds);
				}

				//depending on fade_in or fade_out behave differently:
				if(active_triggers[i].current_frame <= LC_FLASH_FADE_IN_FRAMES){
					//fade in the color as an overlay on all pixels:
					for(uint16_t led=0; led<num_leds; led++){
						//find the closest two LEDs to the offset value of the event:
						if((active_triggers[i].offset >= led_offsets[led]) && (active_triggers[i].offset < led_offsets[led+1])){
							//we will set the color of led and led+1 once we've found an offset that's inbetween them.
							uint16_t led_2 = led + 1;
							//check to make sure the last led isn't too large for the next bit
							if(led_2 >= num_leds){
								led_2 = 0;
							}
							//now map a color for both leds based on the current frames
							uint32_t intermediate_color_1 = color_map(	active_triggers[i].current_frame,
																		0,
																		LC_FLASH_FADE_IN_FRAMES,
																		led_color_array[led],
																		active_triggers[i].color);
							uint32_t intermediate_color_2 = color_map(	active_triggers[i].current_frame,
																		0,
																		LC_FLASH_FADE_IN_FRAMES,
																		led_color_array[led_2],
																		active_triggers[i].color);
							led_color_array[led] = intermediate_color_1;
							led_color_array[led_2] = intermediate_color_2;
							//exit the led for loop once the color has been set.
							break;
						}
					}
				} else {
					//fade in the color as an overlay on all pixels:
					for(uint16_t led=0; led<num_leds; led++){
						//find the closest two LEDs to the offset value of the event:
						if((active_triggers[i].offset >= led_offsets[led]) && (active_triggers[i].offset < led_offsets[led+1])){
							//we will set the color of led and led+1 once we've found an offset that's inbetween them.
							uint16_t led_2 = led + 1;
							//check to make sure the last led isn't too large for the next bit
							if(led_2 >= num_leds){
								led_2 = 0;
							}
							//now map a color for both leds based on the current frames
							uint32_t intermediate_color_1 = color_map(	active_triggers[i].current_frame,
																		LC_FLASH_FADE_IN_FRAMES,
																		active_triggers[i].total_frames,
																		active_triggers[i].color,
																		led_color_array[led]);
							uint32_t intermediate_color_2 = color_map(	active_triggers[i].current_frame,
																		LC_FLASH_FADE_IN_FRAMES,
																		active_triggers[i].total_frames,
																		active_triggers[i].color,
																		led_color_array[led_2]);
							led_color_array[led] = intermediate_color_1;
							led_color_array[led_2] = intermediate_color_2;
							//exit the led for loop once the color has been set.
							break;
						}
					}
				}//fade_in or fade_out
			} else if(active_triggers[i].type == LC_TRIGGER_COLOR_SHOT ||
							active_triggers[i].type == LC_TRIGGER_COLOR_SHOT_SLOW_FADE ||
							active_triggers[i].type == LC_TRIGGER_COLOR_SHOT_RAINBOW ){
				//animate a color shot at the correct offset based on the current_frames for the event.

				//for color shots, we need to calculate the offset as a proportion of the total frames relative to the starting position.
				if(active_triggers[i].direction == LEFT){
					active_triggers[i].offset = map(active_triggers[i].current_frame,
													0,
													active_triggers[i].total_frames,
													LC_DEFAULT_SHOT_POSITION,
													0);
				} else {
					active_triggers[i].offset = map(active_triggers[i].current_frame,
													0,
													active_triggers[i].total_frames,
													LC_DEFAULT_SHOT_POSITION,
													LC_MAX_OFFSET);
				}

				//We can make an array that notes the offset position of each LED:
				//note that it's double the actual number of LEDs to make iteration a single for loop regardless of offset, so it can make it around the circle twice.
				uint32_t led_offsets[num_leds*2];
				led_offsets[0] = 0;
				for(int i=1; i<num_leds*2; i++){
					led_offsets[i] = led_offsets[i-1]+(LC_MAX_OFFSET / num_leds);
				}

				//fade in the color as an overlay on all pixels:
				for(uint16_t led=0; led<num_leds; led++){
					//find the closest two LEDs to the offset value of the event:
					if((active_triggers[i].offset >= led_offsets[led]) && (active_triggers[i].offset < led_offsets[led+1])){
						//we will set the color of led and led+1 once we've found an offset that's inbetween them.
						uint16_t led_2 = led + 1;
						//check to make sure the last led isn't too large for the next bit
						if(led_2 >= num_leds){
							led_2 = 0;
						}
						//now map a color for both leds based on the current frames
						uint32_t intermediate_color_1 = color_map(	active_triggers[i].offset,
																	led_offsets[led],
																	led_offsets[led+1],
																	active_triggers[i].color,
																	led_color_array[led]);
						uint32_t intermediate_color_2 = color_map(	active_triggers[i].offset,
																	led_offsets[led],
																	led_offsets[led+1],
																	led_color_array[led_2],
																	active_triggers[i].color);
						led_color_array[led] = intermediate_color_1;
						led_color_array[led_2] = intermediate_color_2;
						//exit the led for loop once the color has been set.
						break;
					}
				}
			} else if(active_triggers[i].type == LC_TRIGGER_FLASH ||
							active_triggers[i].type == LC_TRIGGER_FLASH_SLOW_FADE ||
							active_triggers[i].type == LC_TRIGGER_FLASH_RAINBOW ){
				//flash all LEDs with an overlayed color based on the current_frames for the event.
				//depending on fade_in or fade_out behave differently:
				if(active_triggers[i].current_frame <= LC_FLASH_FADE_IN_FRAMES){
					//fade in the color as an overlay on all pixels:
					for(uint16_t led=0; led<num_leds; led++){
						uint32_t intermediate_color = color_map(active_triggers[i].current_frame,
																0,
																LC_FLASH_FADE_IN_FRAMES,
																led_color_array[led],
																active_triggers[i].color);
						led_color_array[led] = intermediate_color;
					}
				} else {
					//fade out the color as an overlay on all pixels:
					for(uint16_t led=0; led<num_leds; led++){
						uint32_t intermediate_color = color_map(active_triggers[i].current_frame,
																LC_FLASH_FADE_IN_FRAMES,
																active_triggers[i].total_frames,
																active_triggers[i].color,
																led_color_array[led]);
						led_color_array[led] = intermediate_color;
					}
				}
			}//flash if
		}//num_trigger_events for loop
	}//if num_trigger_events > 0
}//update_trigger_animations()

//this will add a new trigger animation to the active_triggers[] array.
void Animation::add_trigger_event(TriggerEvent event){
	//make sure there is room for another animation
	if(num_trigger_events < MAX_TRIGGER_EVENTS){
		//add it to the end of the line, and icnrement the array.
		active_triggers[num_trigger_events] = event;
		num_trigger_events++;
		#ifdef TRIGGER_DEBUG
			Serial.print("New trigger event added to animation. Currently there are ");
			Serial.print(num_trigger_events);
			Serial.println(" running.");
		#endif
	} else {
		#ifdef TRIGGER_DEBUG
			Serial.println("Trigger Event Buffer is Full - New trigger event has not been added.");
		#endif
	}
}

//this will remove any trigger events whose event_has_completed flag is set to true and shift down the remaining trigger events.
void Animation::clean_trigger_events(){
	//this will need to check the active trigger events and remove one if it is present.
	for(int event_num = 0; event_num<num_trigger_events; event_num++){
		if(active_triggers[event_num].event_has_completed){
			//remove the trigger event from the array and move any remaining trigger events down the line as needed.
			for(int i=event_num; i<num_trigger_events; i++){
				active_triggers[i] = active_triggers[i+1];
			}
			//decrement the num_trigger_events variable
			num_trigger_events--;
			#ifdef TRIGGER_DEBUG
				Serial.print("Trigger event removed from animation. Currently there are ");
				Serial.print(num_trigger_events);
				Serial.println(" running.");
			#endif
		}
	}
}

//this will fill an LED array with a solid color.
void Animation::fill_solid(uint32_t * led_array, uint16_t num_leds, uint32_t color){
	for(int i=0; i<num_leds; i++){
		led_array[i] = color;
	}
}

//this will populate an LED array with a rainbow with an origin based on an offset value,
//and an additional offset based on the number of frames out of a total number of frames.
void Animation::fill_rainbow(uint32_t * led_array, uint16_t num_leds, rainbow rainbow, int32_t origin_offset, uint32_t current_frame, uint32_t total_frames){
	//first we need to use the offset and frame values to determine where the pure rainbow colors begin as an offset from the origin:
	int32_t color_offset_start_position = 0;
	if(total_frames > 0){
		//this is the offset as set externally, plus the ratio of current_frame/total_frames times the total number of possible offset positions.
		color_offset_start_position = correct_offset(origin_offset + current_frame*LC_MAX_OFFSET/total_frames);
	} else {
		//if there is a non-positive number of total_frames, just use the offset alone:
		color_offset_start_position = correct_offset(origin_offset);
	}

	//We can make an array that notes the offset position of each LED:
	//note that it's double the actual number of LEDs to make iteration a single for loop regardless of offset, so it can make it around the circle twice.
	int32_t led_offsets[num_leds*2];
	led_offsets[0] = 0;
	for(int i=1; i<num_leds*2; i++){
		led_offsets[i] = led_offsets[i-1]+(LC_MAX_OFFSET / num_leds);
	}

	//we will also need an offset value between the pure rainbow color positions - this is the distance between each pure rainbow color:
	uint32_t distance_between_rainbow_colors = LC_MAX_OFFSET / rainbow.num_colors;

	//This double rainbow color array will allow for me to use a single loop to iterate through the leds regardless of the offset, as it can make it around the circle twice.
	uint32_t double_rainbow[rainbow.num_colors*2];
	for(int i=0; i<rainbow.num_colors; i++){
		double_rainbow[i] = rainbow.colors[i];
	}
	for(int i=0; i<rainbow.num_colors; i++){
		double_rainbow[i+rainbow.num_colors] = rainbow.colors[i];
	}

	//make an int to track the number of LEDS that have been set.
	uint16_t num_leds_set = 0;
	//and another to tell the for loop to break when all have been set
	bool all_leds_set = false;

	//iterate through the colors starting at the starting LED, and looping back when it hits num_leds
	for(int i=0; i<rainbow.num_colors*2; i++){
		//establish the current and next color. the next color will need to be color 0 on the final iteration.
		uint32_t current_color;
		uint32_t next_color;
		if(i != ( (rainbow.num_colors*2)-1 ) ){
			current_color = double_rainbow[i];
			next_color = double_rainbow[i+1];
		} else {
			current_color = double_rainbow[i];
			next_color = double_rainbow[0];
		}

		//set the current and next color positions.
		int32_t current_color_position = color_offset_start_position + (i*distance_between_rainbow_colors);
		int32_t next_color_position = color_offset_start_position + (i*distance_between_rainbow_colors) + distance_between_rainbow_colors;

		//initialize for use below:
		uint32_t mid_color = 0;

		//iterate through all LEDs and see which LEDs are between the color positions.
		for(int led = 0; led < num_leds*2; led++){
			if(led_offsets[led] >= current_color_position && led_offsets[led] < next_color_position){
				//Should only activate if the LED position is between the current and next LED.
				//set the led number to be set to a number below num_leds
				uint16_t corrected_led = led;
				if(led >= num_leds){
					corrected_led = led - num_leds;
				}
				//this determines the color at the LED based on its position between the pure color offsets.
				mid_color = color_map(led_offsets[led], current_color_position, next_color_position, current_color, next_color);

				//set the led color in the led_offsets
				led_array[corrected_led] = mid_color;

				num_leds_set++;
				//check to see if it's set all of the LEDS. and if so, break from the function
				if(num_leds_set > num_leds){
					all_leds_set = true;
				}
				#ifdef RAINBOW_FILL_DEBUG
				Serial.print("i is: ");
				Serial.print(i);
				Serial.print(" and led is: ");
				Serial.print(led);
				Serial.print(" and current LED position is ");
				Serial.print(led_offsets[led]);
				Serial.print(" and current color position is ");
				Serial.print(current_color_position);
				Serial.print(" and num_leds_set is ");
				Serial.println(num_leds_set);
				#endif

			}
			if(all_leds_set){
				break;
			}
		} //led for loop
	} //rainbow color for loop
}

//this will populate an LED array with a marquee with an origin based on an offset value,
//and an additional offset based on the number of frames out of a total number of frames.
void Animation::fill_marquee(uint32_t * led_array, uint16_t num_leds, uint32_t color, int32_t origin_offset, uint32_t current_frame, uint32_t total_frames){
	//first we need to use the offset and frame values to determine where the first marquee pip begins as an offset from the origin:
	int32_t color_offset_start_position = 0;
	if(total_frames > 0){
		//this is the offset as set externally, plus the ratio of current_frame/total_frames times the total number of possible offset positions.
		color_offset_start_position = correct_offset(origin_offset + current_frame*LC_MAX_OFFSET/total_frames);
	} else {
		//if there is a non-positive number of total_frames, just use the offset alone:
		color_offset_start_position = correct_offset(origin_offset);
	}

	//We can make an array that notes the offset position of each LED:
	//note that it's double the actual number of LEDs to make iteration a single for loop regardless of offset, so it can make it around the circle twice.
	int32_t led_offsets[num_leds*2];
	led_offsets[0] = 0;
	for(int i=1; i<num_leds*2; i++){
		led_offsets[i] = led_offsets[i-1]+(LC_MAX_OFFSET / num_leds);
	}

	//we will also need an offset value between the pips in the marquee - this is a pip-to-pip offset:
	int32_t distance_between_marquee_subdivisions = LC_MAX_OFFSET / marquee_subdivisions;
	//pips can have different on/off relative distances inside each subdivision. This is how big a sub-pip will be, determining which parts of a subdivision are dark vs lit.
	//for instance, if a subdivision is 2000 offset steps, LC_MARQUEE_ON is 1 and LC_MARQUEE_OFF is 3, a subpip will be 1/3 of the whole subdivision, or 500 offset steps.
	//note that one step of a marquee animation is the distance_between_marquee_subpips value
	int32_t distance_between_marquee_subpips = distance_between_marquee_subdivisions / (marquee_on_subpips + marquee_off_subpips);

	//in order for the marquee to snap discretely, we will need to snap the color_offset_start_position value to the nearest subpixel location:
	//due to how integer division works, this will always be the floor value intil the animation is on the next step.
	uint32_t num_subpixels_at_current_offset = color_offset_start_position / distance_between_marquee_subpips;
	color_offset_start_position = num_subpixels_at_current_offset * distance_between_marquee_subpips;

	//we will also need an array storing the colors for each subpixel - this will be twice as long as the number of subpixels to allow for a single iterative loop.
	uint32_t double_marquee[total_marquee_subpips*2];
	for(int i=0; i<total_marquee_subpips*2; i++){
		//since the pattern repeats every (marquee_on_subpips+marquee_off_subpips) subpips, we can use a modulus to only worry about whether the value is more than the number of on pixels to set the array up.
		uint16_t subpixel_number = i % (marquee_on_subpips+marquee_off_subpips);
		if(subpixel_number < marquee_on_subpips){
			double_marquee[i] = color;
		} else {
			double_marquee[i] = off;
		}
	}

	//make an int to track the number of LEDS that have been set.
	uint16_t num_leds_set = 0;
	//and another to tell the for loop to break when all have been set
	bool all_leds_set = false;

	//iterate through the subpixels starting at the starting LED, and looping back when it hits num_leds
	for(int i=0; i<total_marquee_subpips*2; i++){
		//establish the current and next color. the next color will need to be color 0 on the final iteration.
		uint32_t current_color;
		uint32_t next_color;
		if(i != ( (total_marquee_subpips*2)-1 ) ){
			current_color = double_marquee[i];
			next_color = double_marquee[i+1];
		} else {
			current_color = double_marquee[i];
			next_color = double_marquee[0];
		}
		//set the current and next color positions.
		int32_t current_subpip_position = color_offset_start_position + (i*distance_between_marquee_subpips);
		int32_t next_subpip_position = color_offset_start_position + (i*distance_between_marquee_subpips) + distance_between_marquee_subpips;

		//initialize for use below:
		uint32_t mid_color = 0;

		//iterate through all LEDs and see which LEDs are between the color positions.
		for(int led = 0; led < num_leds*2; led++){
			if(led_offsets[led] >= current_subpip_position && led_offsets[led] < next_subpip_position){
				//if the current color is off, leave the pixel alone, but still treat it as a set pixel
				if(current_color != off){
					//Should only activate if the LED position is between the current and next LED.
					//set the led number to be set to a number below num_leds
					uint16_t corrected_led = led;
					if(led >= num_leds){
						corrected_led = led - num_leds;
					}
					//this determines the color at the LED based on its position between the pure color offsets.
					mid_color = color_map(led_offsets[led], current_subpip_position, next_subpip_position, current_color, next_color);

					//set the led color in the led_offsets
					led_array[corrected_led] = mid_color;
				}

				num_leds_set++;
				//check to see if it's set all of the LEDS. and if so, break from the function
				if(num_leds_set > num_leds){
					all_leds_set = true;
				}
				#ifdef MARQUEE_FILL_DEBUG
				Serial.print("i is: ");
				Serial.print(i);
				Serial.print(" and led is: ");
				Serial.print(led);
				Serial.print(" and current LED position is ");
				Serial.print(led_offsets[led]);
				Serial.print(" and current subpip position is ");
				Serial.print(current_color_position);
				Serial.print(" and num_leds_set is ");
				Serial.println(num_leds_set);
				#endif

			}
			if(all_leds_set){
				break;
			}
		} //led for loop
	} //marquee subpixel for loop
}

//this will populate a VU-meter style animation that is entirely dependent on setting the external offset value.
//it wil render the rainbow up to the offset, and leave the background running above the offset
void Animation::fill_vu_meter(uint32_t * led_array, uint16_t num_leds, rainbow rainbow, int32_t origin_offset){
	//we'll need a temporary color array before we overwrite the actual Animation's led_color_array.
	uint32_t temp_color_array[num_leds];

	//first we fill the whole temp array with the rainbow as if the vu meter's offset is at LC_MAX_OFFSET.
	//note that the offset for the vu meter rainbow fill will always be at the default of 0, so the argument is omitted.
	fill_rainbow(temp_color_array, num_leds, rainbow);

	//We can make an array that notes the offset position of each LED:
	//note that it's double the actual number of LEDs to make iteration a single for loop regardless of offset, so it can make it around the circle twice.
	int32_t led_offsets[num_leds*2];
	led_offsets[0] = 0;
	for(int i=1; i<num_leds*2; i++){
		led_offsets[i] = led_offsets[i-1]+(LC_MAX_OFFSET / num_leds);
	}

	//now we iterate through the led positions, and if the led's position is larger than the origin_offset value, we turn the color off.
	for(uint16_t led = 0; led < num_leds; led++){
		if(led_offsets[led] > origin_offset){
			temp_color_array[led] = off;
		} else {
			//Some special first/last pixel conditions:
			if( (led == 0) && ( origin_offset < ((LC_MAX_OFFSET / num_leds)/2) ) ){
				temp_color_array[led] = off;
			}
			//set the last one to the final rainbow color every time, if it is on.
			if(led == (num_leds-1)){
				temp_color_array[led] = rainbow.colors[rainbow.num_colors-1];
			}
		}
	}

	//finally, we overwrite the values in the led_array that are not set to off.
	for(uint16_t led = 0; led < num_leds; led++){
		if(temp_color_array[led] != off){
			led_array[led] = temp_color_array[led];
		} else {
			//the color will remain the background color that was already present in led_array.
		}
	}
}

//this will return an adjusted color based on the current/total frame ratio between two colors of a rainbow.
uint32_t Animation::slow_fade_color_adjust(rainbow rainbow, uint8_t current_color_position, uint32_t current_frame, uint32_t total_frames){
	uint32_t current_color = rainbow.colors[current_color_position];
	uint8_t next_color_position = current_color_position + 1;
	if( (next_color_position) >= rainbow.num_colors){
		next_color_position = 0;
	}
	uint32_t next_color = rainbow.colors[next_color_position];
	uint32_t adjusted_color = color_map(current_frame, 0, total_frames, current_color, next_color);
	return adjusted_color;
}

//this will increment the current_frames of all types until total_frames and then reset it to 0.
void Animation::increment_frame(){
	//increment background related info
	current_bg_frame++;
	if(current_bg_frame >= total_bg_frames){
		current_bg_frame = 0;
		//depending on the mode, increment the rainbow when the frames overflow so the rainbow will advance to the next color:
		if(bg_mode == LC_BG_SLOW_FADE){
			increment_bg_rainbow();
		}
	}
	//increment foreground related info
	current_fg_frame++;
	if(current_fg_frame >= total_fg_frames){
		current_fg_frame = 0;
	}

	//increment marquee slow_fade related frames
	current_marquee_fade_frame++;
	if(current_marquee_fade_frame >= total_marquee_fade_frames){
		current_marquee_fade_frame = 0;
		//depending on the mode, increment the rainbow when the frames overflow so the rainbow will advance to the next color:
		if(fg_mode == LC_FG_MARQUEE_SLOW_FADE){
			increment_fg_rainbow();
		}
	}

	//increment trigger slow_fade related frames
	current_trigger_fade_frame++;
	if(current_trigger_fade_frame >= total_trigger_fade_frames){
		current_trigger_fade_frame = 0;
		//trigger event flow fades all occur on the same rainbow, so they are tracked together.
		increment_trigger_rainbow();
	}

	//increment all current trigger event frames and clean finished events as needed.
	for(int i=0; i<num_trigger_events; i++){
		active_triggers[i].current_frame++;
		if(active_triggers[i].current_frame >= active_triggers[i].total_frames){
			active_triggers[i].event_has_completed = true;
		}
	}
	clean_trigger_events();
}

//this will increment the current current_bg_rainbow_color until overflow and then return to color 0
void Animation::increment_bg_rainbow(){
	current_bg_rainbow_color++;
	if(current_bg_rainbow_color >= bg_rainbow.num_colors){
		current_bg_rainbow_color = 0;
	}
}

//this will increment the current current_bg_rainbow_color until overflow and then return to color 0
void Animation::increment_fg_rainbow(){
	current_fg_rainbow_color++;
	if(current_fg_rainbow_color >= fg_rainbow.num_colors){
		current_fg_rainbow_color = 0;
	}
}

//this will increment the current current_trigger_rainbow_color until overflow and then return to color 0
void Animation::increment_trigger_rainbow(){
	current_trigger_rainbow_color++;
	if(current_trigger_rainbow_color >= trigger_rainbow.num_colors){
		current_trigger_rainbow_color = 0;
	}
}

//this will adjust the offset number so that it is always within bounds
uint32_t Animation::correct_offset(int32_t origin_offset){
	int32_t corrected_offset = origin_offset;
	//first check for and correct any out of bounds offsets.
	while(corrected_offset < LC_MAX_OFFSET*(-1)){
		corrected_offset = corrected_offset + LC_MAX_OFFSET;
	}
	while(corrected_offset > LC_MAX_OFFSET){
		corrected_offset = corrected_offset - LC_MAX_OFFSET;
	}
	//will need to take into account negative offsets for manual animations done by changing the offset directly. This can be done by inverting the offset at the start:
	if(corrected_offset < 0){
		corrected_offset = corrected_offset + LC_MAX_OFFSET;
	}
	return corrected_offset;
}


/* ----- END PRIVATE FUNCTIONS ----- */
/* ----- END ANIMATION CLASS FUNCTIONS -----*/

/* ----- BEGIN LIGHTINGCONTROL CLASS FUNCTIONS -----*/

//constructor function
LightingControl::LightingControl(Adafruit_NeoPixel *LED_controller, uint8_t max_brightness){
	//set the strip variable for the controller to the referenced Adafruit_NeoPixel object
	strip = LED_controller;
	brightness = max_brightness;
}

/* ----- PUBLIC FUNCTIONS BELOW ----- */

//this should be called during setup to initialize and blank the Adafruit_NeoPixel led strip prior to doing anything else.
void LightingControl::init(){
	//this just makes sure the strip.begin() function got called.
	strip->begin();
	//set the brightness or the entire controller once, ever.
	strip->setBrightness(brightness);
	//shows the initially empty strip
	strip->show();
	//set the number of current animations to 0.
	num_current_animations = 0;
}

//This needs to be called regularly to update all the animations on the lighting controller
int LightingControl::update(){
	if(last_update > LC_DEFAULT_REFRESH_RATE){
		//this will iterate through any active Animation objects and update the LEDs under the Animation object's control
		for(int i=0; i<num_current_animations; i++){
			//first update the animation to get the current led_color_array updated.
			currently_running_animations[i]->update();
			//then set the pixels on the strip object:
			for(int l = 0; l<currently_running_animations[i]->num_leds; l++){
				set_corrected_color(currently_running_animations[i]->led_positions[l], currently_running_animations[i]->led_color_array[l]);
			}
		}
		//show the pixels once the whole set of Animations has been updated.
		strip->show();
		//reset the last_update
		last_update = 0;
		return LC_STRIP_WRITTEN;
	}
	return LC_STRIP_NOT_WRITTEN;
}

//this will add an animaton to the lighting controller so the update function will know which animations to run.
void LightingControl::add_animation(Animation * new_animation){
	if(num_current_animations < MAX_ANIMATIONS){
		//this will need to add the animation if it is not present, and then it will still need to call an Animation::init() even if it is already present.
		int16_t animation_position = check_animations(new_animation);
		//if it's not already in the array.
		if(animation_position == NOT_IN_ARRAY){
			//set the note as the next item in the array.
			currently_running_animations[num_current_animations] = new_animation;
			num_current_animations++;
		} else {
			//if it is in the array, shift the others forward and move it to the end.
			for(int i=animation_position; i<num_current_animations; i++){
				currently_running_animations[i] = currently_running_animations[i+1];
			}
			currently_running_animations[num_current_animations-1] = new_animation;
		}
		#ifdef ANIMATION_DEBUG
			Serial.print("New animation added to lighting controller. Currently there are ");
			Serial.print(num_current_animations);
			Serial.println(" running.");
		#endif
	} else {
		#ifdef ANIMATION_DEBUG
			Serial.print("Animation Buffer is Full - New Animation has not been added.");
		#endif
	}
}

//this will remove an animation from the lighting controller if it's presently on it, causing the animation to stop operating on the LEDs.
void LightingControl::rm_animation(Animation * animation_to_be_removed){
	//this will need to check the active animations and remove one if it is present.
	int16_t animation_position = check_animations(animation_to_be_removed);
	if(animation_position != NOT_IN_ARRAY){
		//first turn all the leds in the animation off.
		for(int l = 0; l<currently_running_animations[animation_position]->num_leds; l++){
			set_corrected_color(currently_running_animations[animation_position]->led_positions[l], off);
		}
		//then remove the animation from the array and move any animations down the line as needed.
		for(int i=animation_position; i<num_current_animations; i++){
			currently_running_animations[i] = currently_running_animations[i+1];
		}
		//finally decrement the num_current_animations variable
		num_current_animations--;
		#ifdef ANIMATION_DEBUG
			Serial.print("Animation removed from lighting controller. Currently there are ");
			Serial.print(num_current_animations);
			Serial.println(" running.");
		#endif
	} else {
		//do nothing
	}
}

/* ----- END PUBLIC FUNCTIONS ----- */
/* ----- PRIVATE FUNCTIONS BELOW ----- */

//this is a wrapper function to send corrected color values to the controller.
//It is recommended to use this to send animation updates instead of sending them directly using strip.SetPixelColor().
void LightingControl::set_corrected_color(uint16_t led, uint32_t color){
	uint8_t red = (uint8_t)(color >> 16);
	uint8_t green = (uint8_t)(color >> 8);
	uint8_t blue = (uint8_t)(color);
	strip->setPixelColor(led, pgm_read_byte(&gamma8[red]), pgm_read_byte(&gamma8[green]), pgm_read_byte(&gamma8[blue]));
}

//this will check the currently_running_animations array to see if an animation object is already present:
int16_t LightingControl::check_animations(Animation * animation){
	for(int i=0; i<num_current_animations; i++){
		if(currently_running_animations[i] == animation){
			return i;
		}
	}
	//if none of the above worked, return NOT_IN_ARRAY
	return NOT_IN_ARRAY;
}

/* ----- END PRIVATE FUNCTIONS ----- */
/* ----- END LIGHTINGCONTROL CLASS FUNCTIONS -----*/