//This is just to offload color definitions to another file so I don't need to scroll so much in the main file.

#include "colors.h"

//simple function for converting RGB into uint32_t used by Adafruit_NeoPixel
uint32_t Color(uint8_t r, uint8_t g, uint8_t b){
	return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

//this is a general purpose function for mapping a color transition between two colors (uint32_t) based on a desired position, a starting position, and an ending position.
uint32_t color_map(int x, int in_min, int in_max, uint32_t start_color, uint32_t end_color){

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