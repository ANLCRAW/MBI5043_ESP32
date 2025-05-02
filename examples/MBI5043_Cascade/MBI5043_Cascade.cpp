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

#define SDO 27
#define DCLK 32
#define LE 33
#define GCLK 5
#define STRIP_1 18
#define STRIP_2 22

MBI5043 MBI(SDO, DCLK, LE, GCLK, STRIP_1); // MBI pin nr's

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
	Serial.begin(115200);

	MBI.spi_init(); // Initialize MBI5043
	MBI.setupGCLK(GCLK_40MHZ); // Set GCLK speed

	clear_data();
	MBI.update(NUM_CHIPS_PER_STRIP, pwm_data1, pwm_data2); // Send pwm_data to MBI5043

	MBI.write_config(GCLK_SHIFT_0 | CURRENT_GAIN_ADJUST_100, STRIP_2, NUM_CHIPS_PER_STRIP);	// (see header file of lib), 1st number: GCLK_SHIFT_0 | 2nd number: current gain
	delay(5000);
	Serial.printf("register: 0x%04x\n",MBI.read_config());
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
			MBI.update(NUM_CHIPS_PER_STRIP, pwm_data1, pwm_data2); // Send pwm_data to MBI5043
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
		}
	}
	MBI.update(NUM_CHIPS_PER_STRIP, pwm_data1, pwm_data2); // Send pwm_data to MBI5043
	delay(5000);
	clear_data();
	MBI.update(NUM_CHIPS_PER_STRIP, pwm_data1, pwm_data2); // Send pwm_data to MBI5043
	delay(5000);
}


uint16_t getRainbowChannel(uint8_t pos, char channel) {
	pos %= 256;
	switch (channel) {
	  case 'r':
		if (pos < 85) return (pos * 3 * BRIGHTNESS_MAX) / 255;
		if (pos < 170) return ((255 - (pos - 85) * 3) * BRIGHTNESS_MAX) / 255;
		return 0;
	  case 'g':
		if (pos < 85) return ((255 - pos * 3) * BRIGHTNESS_MAX) / 255;
		if (pos < 170) return 0;
		return ((pos - 170) * 3 * BRIGHTNESS_MAX) / 255;
	  case 'b':
		if (pos < 85) return 0;
		if (pos < 170) return ((pos - 85) * 3 * BRIGHTNESS_MAX) / 255;
		return ((255 - (pos - 170) * 3) * BRIGHTNESS_MAX) / 255;
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
  
	MBI.update(NUM_CHIPS_PER_STRIP, pwm_data1, pwm_data2); // Send pwm_data to MBI5043
	delay(FADE_DELAY);
	phase = (phase + CHASE_SPEED) % 256;
  }


  void loop(void){
	colorTest();
	//fullTest();
	//rainbowChase();
  }