#include "Arduino.h"
#include "MBI5043_ESP32.h"

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

MBI5043 MBI(18, 27, 32, 33, 5); // spi_out (SDI), spi_in (SDO), spi_clk (DCLK), spi_latch (LE), GCLK pin --- cant use pin 35&34 for SDI&GCLK ---

uint32_t pwm_data[48]; //NUM_CHIPS*16 = 2*16 =32

//Pixel order (for now 48 pixels)
uint8_t pixelArray[] = {3,0,6,12,9,15,21,18,24,30,27,33,39,36,42,1,45,4,10,7,13,19,16,22,28,25,31,37,34,40,46,43,2,8,5,11,17,14,20,26,23,29,35,32,38,44,41,47};

void clear_data(void){
	memset(pwm_data, 0x00, sizeof(pwm_data)); // Set all the value's of pwm_data to zero
}

void setup(void){
  clear_data(); // Turn all the led's off
  MBI.spi_init(); // Initialize MBI5043

	//
	// The current-set resistor 'R-EXT' of the MBI5043 should be chosen such
	// that the maximum current is only reached if the digital current gain
	// is set to '2'. That way the chip should always operate within specs.
	// 
	// The chip starts up with a gain setting of '1', which means only half
	// of the maximum current is available. Next we'll change the configuration
	// register to get a gain of '2' for full brightness.
	//
  MBI.write_config(0x0000, CURRENT_GAIN_AJUST_200, TOTAL_NUM_CHIPS);	// 1st number: blank configuration bits (see header file of lib), 2nd number: current gain
}

void loop(void)
{
  for(int i=0; i<TOTAL_NUM_PIXELS;i++){
    pwm_data[pixelArray[i]] = BRIGHTNESS_MAX; // Set value pwm_data
    MBI.update(pwm_data,TOTAL_NUM_CHIPS); // Send pwm_data to MBI5043
    delay(500); // Wait for 500ms for the next color
    clear_data(); // Turn all the led's off
  }
    
}
