#include "Arduino.h"
#include "MBI5043_ESP32.h"

#define BRIGHTNESS_MAX 65535/2 
#define BRIGHTNESS_STEPSIZE 256
//#define FADE_DELAY 5

#define NUM_STRIPS 2
#define NUM_CHIPS_PER_STRIP 3
#define NUM_LEDS_PER_STRIP 16
#define NUM_COLORS 3
#define NUM_PIXELS_PER_STRIP NUM_LEDS_PER_STRIP*NUM_COLORS
#define TOTAL_NUM_PIXELS     NUM_LEDS_PER_STRIP*NUM_COLORS*NUM_STRIPS // 
#define TOTAL_NUM_LEDS     	 NUM_LEDS_PER_STRIP*NUM_STRIPS            // 
#define TOTAL_NUM_CHIPS      NUM_STRIPS*NUM_CHIPS_PER_STRIP

#define FADE_DELAY 20
#define CHASE_SPEED 5         // Lower = faster
#define RAINBOW_LENGTH 96     // Number of colors in the rainbow wheel
#define MAX_BRIGHTNESS 0xffff/2

MBI5043 MBI(27, 32, 33, 5, 18, 22); // spi_in (SDO), spi_clk (DCLK), spi_latch (LE), GCLK pin, spi_out (SDI)

uint16_t pwm_data1[NUM_PIXELS_PER_STRIP]; //NUM_CHIPS*16 = 2*16 =32 * NUM_COLORS = 96
uint16_t pwm_data2[NUM_PIXELS_PER_STRIP]; //NUM_CHIPS*16 = 2*16 =32 * NUM_COLORS = 96

uint8_t pixelArray1[NUM_PIXELS_PER_STRIP] = {3,0,6,12,9,15,21,18,24,30,27,33,39,36,42,1,45,4,10,7,13,19,16,22,28,25,31,37,34,40,46,43,2,8,5,11,17,14,20,26,23,29,35,32,38,44,41,47};
uint8_t pixelArray2[NUM_PIXELS_PER_STRIP] = {44,41,47,35,32,38,26,23,29,17,14,20,8,5,11,46,43,2,37,34,40,28,25,31,19,16,22,10,7,13,1,45,4,39,36,42,30,27,33,21,18,24,12,9,15,3,0,6};

void clear_data(void){
	memset(pwm_data1, 0x00, sizeof(pwm_data1));
  	memset(pwm_data2, 0x00, sizeof(pwm_data2));
}

void full_data(void){
	memset(pwm_data1, 0xff, sizeof(pwm_data1));
  	memset(pwm_data2, 0xff, sizeof(pwm_data2));
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
  MBI.write_config(0x0000, CURRENT_GAIN_ADJUST_200, TOTAL_NUM_CHIPS);	// 1st number: blank configuration bits (see header file of lib), 2nd number: current gain
}



void colorTest(){
	for(int x = 0; x < 2; x++){ // 10 times
		for(int i=0; i<TOTAL_NUM_PIXELS;i++){
			if(i<NUM_PIXELS_PER_STRIP){
				pwm_data1[pixelArray1[i]] = BRIGHTNESS_MAX; // Set value pwm_data
			} else{
				pwm_data2[pixelArray2[i-NUM_PIXELS_PER_STRIP]] = BRIGHTNESS_MAX; // Set value pwm_data
				//Serial.printf("pixelArray = %i\npwm_data2 = %i\n", i-NUM_PIXELS_PER_STRIP, pixelArray2[i-NUM_PIXELS_PER_STRIP]);
			}
			MBI.update(pwm_data1, pwm_data2,  NUM_CHIPS_PER_STRIP); // Send pwm_data to MBI5043
			delay(100); // Wait for 500ms for the next color
			clear_data(); // Turn all the led's off
		}
	}
}

void fullTest(){
	for(int i=0; i<TOTAL_NUM_PIXELS;i++){
		if(i<NUM_PIXELS_PER_STRIP){
			pwm_data1[pixelArray1[i]] = {0xffff};  // Full brightness
		} else{
			pwm_data2[pixelArray2[i-NUM_PIXELS_PER_STRIP]] = {0xffff};  // Full brightness
			//Serial.printf("pixelArray = %i\npwm_data2 = %i\n", i-NUM_PIXELS_PER_STRIP, pixelArray2[i-NUM_PIXELS_PER_STRIP]);
		}
	}
	MBI.update(pwm_data1, pwm_data2, NUM_CHIPS_PER_STRIP); // 3 chips
	delay(15000);
	clear_data();
}

uint16_t getRainbowChannel(uint8_t pos, char channel) {
	pos %= 256;
	switch (channel) {
	  case 'r':
		if (pos < 85) return (pos * 3 * MAX_BRIGHTNESS) / 255;
		if (pos < 170) return ((255 - (pos - 85) * 3) * MAX_BRIGHTNESS) / 255;
		return 0;
	  case 'g':
		if (pos < 85) return ((255 - pos * 3) * MAX_BRIGHTNESS) / 255;
		if (pos < 170) return 0;
		return ((pos - 170) * 3 * MAX_BRIGHTNESS) / 255;
	  case 'b':
		if (pos < 85) return 0;
		if (pos < 170) return ((pos - 85) * 3 * MAX_BRIGHTNESS) / 255;
		return ((255 - (pos - 170) * 3) * MAX_BRIGHTNESS) / 255;
	}
	return 0;
  }
  
  void rainbowChase() {
	static uint8_t phase = 0;
  
	for (int i = 0; i < TOTAL_NUM_PIXELS; i += 3) {  // step by 3 channels per pixel
	  uint8_t pos = (phase + (i / 3) * (256 / (TOTAL_NUM_PIXELS / 3))) % 256;
	  uint16_t r = getRainbowChannel(pos, 'r');
	  uint16_t g = getRainbowChannel(pos, 'g');
	  uint16_t b = getRainbowChannel(pos, 'b');
  
	  // Strip 1
	  if (i < NUM_PIXELS_PER_STRIP) {
	  pwm_data1[pixelArray1[i + 0]] = r;
	  pwm_data1[pixelArray1[i + 1]] = g;
	  pwm_data1[pixelArray1[i + 2]] = b;
	  } else {
	  // Strip 2
	  pwm_data2[pixelArray2[i + 0]] = r;
	  pwm_data2[pixelArray2[i + 1]] = g;
	  pwm_data2[pixelArray2[i + 2]] = b;
	  }
	}
  
	MBI.update(pwm_data1, pwm_data2, NUM_CHIPS_PER_STRIP);
	delay(FADE_DELAY);
	phase = (phase + CHASE_SPEED) % 256;
  }

  void loop(void)
{
	//colorTest();
	//fullTest();
	rainbowChase();
		
}