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

#include <colors.h>

uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
{
	return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

uint32_t color_map(int x, int in_min, int in_max, uint32_t start_color, uint32_t end_color)
{

	//assign the colors based on the above.
	uint32_t start_red = (uint8_t)(start_color >> 16);
	uint32_t start_green = (uint8_t)(start_color >> 8);
	uint32_t start_blue = (uint8_t)(start_color);
	uint32_t end_red = (uint8_t)(end_color >> 16);
	uint32_t end_green = (uint8_t)(end_color >> 8);
	uint32_t end_blue = (uint8_t)(end_color);

	//do the map thing to get the color between the two based on LM_SLOW_FADE_FRAMES
	uint32_t mid_red = map(x, in_min, in_max, start_red, end_red);
	uint32_t mid_green = map(x, in_min, in_max, start_green, end_green);
	uint32_t mid_blue = map(x, in_min, in_max, start_blue, end_blue);

	//finally assign the mid_color.
	uint32_t mid_color = Color(mid_red, mid_green, mid_blue);
	return mid_color;
}