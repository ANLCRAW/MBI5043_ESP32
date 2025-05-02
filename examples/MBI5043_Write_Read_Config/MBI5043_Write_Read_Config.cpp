#include "Arduino.h"
#include "MBI5043_ESP32.h"

#define NUM_CHIPS_PER_STRIP 3

#define SDO 27
#define DCLK 32
#define LE 33
#define GCLK 5
#define SDI 22

MBI5043 MBI(SDO, DCLK, LE, GCLK, SDI); // MBI pin nr's


void setup(void){
	Serial.begin(115200);

	MBI.spi_init(); // Initialize MBI5043

	MBI.write_config(GCLK_SHIFT_0 | CURRENT_GAIN_ADJUST_100, SDI, NUM_CHIPS_PER_STRIP);	// (see header file of lib), 1st number: GCLK_SHIFT_0 | 2nd number: current gain
	
	delay(5000);
	
	Serial.printf("register: 0x%04x\n",MBI.read_config());
	Serial.printf("pre-register: 0x%04x\n",MBI.get_preloaded_config());
}

void loop(void){

}