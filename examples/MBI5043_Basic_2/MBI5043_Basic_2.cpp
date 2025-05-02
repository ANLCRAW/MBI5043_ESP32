#include "Arduino.h"
#include "MBI5043_ESP32.h"

#define BRIGHTNESS_MAX 65535/2 
#define BRIGHTNESS_STEPSIZE 256

#define NUM_STRIPS 1
#define NUM_CHIPS_PER_STRIP 3
#define NUM_LEDS_PER_STRIP 16
#define NUM_COLORS 3
#define NUM_PIXELS_PER_STRIP NUM_LEDS_PER_STRIP*NUM_COLORS
#define TOTAL_NUM_PIXELS     NUM_LEDS_PER_STRIP*NUM_COLORS*NUM_STRIPS // 
#define TOTAL_NUM_LEDS     	 NUM_LEDS_PER_STRIP*NUM_STRIPS            // 
#define TOTAL_NUM_CHIPS      NUM_STRIPS*NUM_CHIPS_PER_STRIP

#define SDO 27
#define DCLK 32
#define LE 33
#define GCLK 5
#define STRIP_1 18

MBI5043 MBI(SDO, DCLK, LE, GCLK, STRIP_1); // MBI pin nr's

uint16_t pwm_data[NUM_PIXELS_PER_STRIP]; //NUM_CHIPS*16 = 2*16 =32 * NUM_COLORS = 96

uint8_t pixelArray[NUM_PIXELS_PER_STRIP] = {3,0,6,12,9,15,21,18,24,30,27,33,39,36,42,1,45,4,10,7,13,19,16,22,28,25,31,37,34,40,46,43,2,8,5,11,17,14,20,26,23,29,35,32,38,44,41,47};

void clear_data(void){
	memset(pwm_data, 0x00, sizeof(pwm_data));
}

void setup(void){
	Serial.begin(115200);

	MBI.spi_init(); // Initialize MBI5043
	MBI.setupGCLK(GCLK_40MHZ); // Set GCLK speed
}

void colorTest(){
	for(int x = 0; x < 2; x++){ // 10 times
		for(int i=0; i<TOTAL_NUM_PIXELS;i++){
			pwm_data[pixelArray[i]] = BRIGHTNESS_MAX; // Set value pwm_data
			MBI.update(NUM_CHIPS_PER_STRIP, pwm_data); // Send pwm_data to MBI5043
			delay(100); // Wait for 500ms for the next color
			clear_data(); // Turn all the led's off
		}
	}
}

void loop(void){
	colorTest();
  }