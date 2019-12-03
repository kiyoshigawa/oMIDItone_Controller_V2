/*
This is a quick library that provides some commonly used functions for using 
32bit colors like in the Adafruit_NeoPixel library. I also have a bunch of
standard color definitions and an array of rainbows for use in lighting projects

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

#ifndef colors_h
#define colors_h

#include <Arduino.h>

//this is the most colors a single rainbow can have
#define MAX_RAINBOW_COLORS 12

//a structure for holding rainbows called rainbow:
struct rainbow {
	uint32_t colors[MAX_RAINBOW_COLORS];
	uint8_t num_colors;
};

//simple function for converting RGB into uint32_t used by Adafruit_NeoPixel
uint32_t Color(uint8_t r, uint8_t g, uint8_t b);

//this is a general purpose function for mapping a color transition between two colors (uint32_t) based on a desired position, a starting position, and an ending position.
uint32_t color_map(int x, int in_min, int in_max, uint32_t start_color, uint32_t end_color);

//named color definitions:
const uint32_t red = Color(255, 0, 0);
const uint32_t orange = Color(255, 127, 0);
const uint32_t yellow = Color(255, 255, 0);
const uint32_t yellow_green = Color(127, 255, 0);
const uint32_t green = Color(0, 255, 0);
const uint32_t green_blue = Color(0, 255, 127);
const uint32_t sky_blue = Color(0, 255, 255);
const uint32_t deep_blue = Color(0, 127, 255);
const uint32_t blue = Color(0, 0, 255);
const uint32_t purple_blue = Color(127, 0, 255);
const uint32_t purple = Color(255, 0, 255);
const uint32_t dark_purple = Color(255, 0, 127);

const uint32_t white = Color(255,255,255);
const uint32_t off = Color(0, 0, 0);

//a couple 'rainbows' for simple on/off functionality.
const rainbow r_off = {
	.colors = {
		off
	},
	.num_colors = 1
};

const rainbow r_on = {
	.colors = {
		white
	},
	.num_colors = 1
};

const rainbow r_red = {
	.colors = {
		red
	},
	.num_colors = 1
};

const rainbow r_orange = {
	.colors = {
		orange
	},
	.num_colors = 1
};

const rainbow r_yellow = {
	.colors = {
		yellow
	},
	.num_colors = 1
};

const rainbow r_yellow_green = {
	.colors = {
		yellow_green
	},
	.num_colors = 1
};

const rainbow r_green = {
	.colors = {
		green
	},
	.num_colors = 1
};

const rainbow r_green_blue = {
	.colors = {
		green_blue
	},
	.num_colors = 1
};

const rainbow r_sky_blue = {
	.colors = {
		sky_blue
	},
	.num_colors = 1
};

const rainbow r_deep_blue = {
	.colors = {
		deep_blue
	},
	.num_colors = 1
};

const rainbow r_blue = {
	.colors = {
		blue
	},
	.num_colors = 1
};

const rainbow r_purple_blue = {
	.colors = {
		purple_blue
	},
	.num_colors = 1
};

const rainbow r_purple = {
	.colors = {
		purple
	},
	.num_colors = 1
};

const rainbow r_dark_purple = {
	.colors = {
		dark_purple
	},
	.num_colors = 1
};

const rainbow r_dark_grey = {
	.colors = {
		Color(32,32,32)
	},
	.num_colors = 1
};

//recommended for use with LC_FG_VU_METER modes on a LightingControl setup, but not required.
//Adjust the ratio of green to yellow to red to your liking.
const rainbow r_vu = {
	.colors = {
		green,
		green,
		green,
		green,
		green,
		green,
		yellow,
		yellow,
		red
	},
	.num_colors = 9
};


//red and dark red pattern
const rainbow r_dark_red_pattern = {
	.colors = {
		Color(127,0,0),
		Color(64,0,0),
		Color(127,0,0),
		Color(64,0,0),
		Color(127,0,0),
		Color(64,0,0),
	},
	.num_colors = 6
};

//yellow and dark yellow pattern
const rainbow r_dark_yellow_pattern = {
	.colors = {
		Color(127,127,0),
		Color(64,64,0),
		Color(127,127,0),
		Color(64,64,0),
		Color(127,127,0),
		Color(64,64,0),
	},
	.num_colors = 6
};

//sky blue and dark sky blue pattern
const rainbow r_dark_sky_blue_pattern = {
	.colors = {
		Color(0,127,127),
		Color(0,64,64),
		Color(0,127,127),
		Color(0,64,64),
		Color(0,127,127),
		Color(0,64,64),
	},
	.num_colors = 6
};

//purple and dark purple pattern
const rainbow r_dark_purple_pattern = {
	.colors = {
		Color(127,0,127),
		Color(64,0,64),
		Color(127,0,127),
		Color(64,0,64),
		Color(127,0,127),
		Color(64,0,64),
	},
	.num_colors = 6
};

//green and dark green pattern
const rainbow r_dark_green_pattern = {
	.colors = {
		Color(0,127,0),
		Color(0,64,0),
		Color(0,127,0),
		Color(0,64,0),
		Color(0,127,0),
		Color(0,64,0),
	},
	.num_colors = 6
};

//blue and dark blue pattern
const rainbow r_dark_blue_pattern = {
	.colors = {
		Color(0,0,127),
		Color(0,0,64),
		Color(0,0,127),
		Color(0,0,64),
		Color(0,0,127),
		Color(0,0,64),
	},
	.num_colors = 6
};

//white and grey pattern
const rainbow r_white_grey_pattern = {
	.colors = {
		white,
		Color(64,64,64),
		white,
		Color(64,64,64),
		white,
		Color(64,64,64)
	},
	.num_colors = 6
};

//this is the traditional roygbiv rainbow pattern
const rainbow r_roygbiv = {
	.colors = {
		red,
		yellow,
		green,
		sky_blue,
		blue,
		purple
	},
	.num_colors = 6
};

//rainbow is a double rainbow of r3
const rainbow r_double_roygbiv = {
	.colors = {
		red,
		yellow,
		green,
		sky_blue,
		blue,
		purple,
		red,
		yellow,
		green,
		sky_blue,
		blue,
		purple
	},
	.num_colors = 12
};

//the primary colors red, blue and yellow:
const rainbow r_rby = {
	.colors = {
		red,
		off,
		yellow,
		off,
		blue,
		off
	},
	.num_colors = 6
};

//the secondary colors orange, green and purple:
const rainbow r_ogp = {
	.colors = {
		off,
		orange,
		off,
		green,
		off,
		purple
	},
	.num_colors = 6
};

//red green and blue
const rainbow r_rgb = {
	.colors = {
		off,
		red,
		off,
		green,
		off,
		blue
	},
	.num_colors = 6
};

//blue and yellow
const rainbow r_by = {
	.colors = {
		off,
		yellow,
		off,
		blue,
		off,
		yellow,
		off,
		blue
	},
	.num_colors = 8
};

//red and sky_blue
const rainbow r_rb = {
	.colors = {
		off,
		red,
		off,
		sky_blue,
		off,
		red,
		off,
		sky_blue
	},
	.num_colors = 8
};

//Orange and deep_blue
const rainbow r_ob = {
	.colors = {
		off,
		orange,
		off,
		deep_blue,
		off,
		orange,
		off,
		deep_blue
	},
	.num_colors = 8
};

//blue and white pattern
const rainbow r_bw = {
	.colors = {
		white,
		sky_blue,
		blue,
		deep_blue,
		white,
		sky_blue,
		blue,
		deep_blue
	},
	.num_colors = 8
};

//red and white pattern
const rainbow r_rw = {
	.colors = {
		white,
		Color(127,0,0),
		red,
		Color(127,0,0),
		white,
		Color(127,0,0),
		red,
		Color(127,0,0)
	},
	.num_colors = 8
};

//green and white pattern
const rainbow r_gw = {
	.colors = {
		white,
		Color(0,127,0),
		green,
		Color(0,127,0),
		white,
		Color(0,127,0),
		green,
		Color(0,127,0)
	},
	.num_colors = 8
};

//this is an array of all the rainbows above for use in whatever lighting animation switchers you have:
const uint16_t num_rainbows = 34;
const rainbow rb_array[num_rainbows] = {
	r_off,                    //0
	r_on,                     //1
	r_vu,                     //2
	r_red,                    //3
	r_orange,                 //4
	r_yellow,                 //5
	r_yellow_green,           //6
	r_green,                  //7
	r_green_blue,             //8
	r_sky_blue,               //9
	r_deep_blue,              //10
	r_blue,                   //11
	r_purple_blue,            //12
	r_purple,                 //13
	r_dark_purple,            //14
	r_dark_grey,              //15
	r_dark_red_pattern,       //16
	r_dark_yellow_pattern,    //17
	r_dark_sky_blue_pattern,  //18
	r_dark_purple_pattern,    //19
	r_dark_green_pattern,     //20
	r_dark_blue_pattern,      //21
	r_white_grey_pattern,     //22
	r_roygbiv,                //23
	r_double_roygbiv,         //24
	r_rby,                    //25
	r_ogp,                    //26
	r_rgb,                    //27
	r_by,                     //28
	r_rb,                     //29
	r_ob,                     //30
	r_bw,                     //31
	r_rw,                     //32
	r_gw,                     //33
};
#endif