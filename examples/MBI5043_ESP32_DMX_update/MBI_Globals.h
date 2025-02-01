#ifndef _MBI_Globals_H_
#define _MBI_Globals_H_

#include "MBI5043_ESP32.h"

  MBI5043 MBI(13, 27, 32, 33, 12); // spi_out (SDI), spi_in (SDO), spi_clk (DCLK), spi_latch (LE), GCLK pin

  #define BRIGHTNESS_MAX 65535
  #define BRIGHTNESS_STEPSIZE 256
  #define FADE_DELAY 10

  #define NUM_STRIPS 1
  #define NUM_CHIPS_PER_STRIP 3
  #define NUM_LEDS_PER_STRIP 16
  #define NUM_COLORS 3
  #define NUM_PIXELS_PER_STRIP NUM_LEDS_PER_STRIP*NUM_COLORS
  #define TOTAL_NUM_PIXELS     NUM_LEDS_PER_STRIP*NUM_COLORS*NUM_STRIPS // 
  #define TOTAL_NUM_LEDS     	 NUM_LEDS_PER_STRIP*NUM_STRIPS            // 
  #define TOTAL_NUM_CHIPS      NUM_STRIPS*NUM_CHIPS_PER_STRIP

  uint16_t pwm_data[48]; //NUM_CHIPS*16 = 2*16 =32

  //Pixel order (for now 48 pixels)
  uint8_t pixelArray[] = {3,0,6,12,9,15,21,18,24,30,27,33,39,36,42,1,45,4,10,7,13,19,16,22,28,25,31,37,34,40,46,43,2,8,5,11,17,14,20,26,23,29,35,32,38,44,41,47};

#endif