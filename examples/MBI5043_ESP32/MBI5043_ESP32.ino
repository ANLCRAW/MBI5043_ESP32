
#include <MBI5043_ESP32.h>
//#include <MBI5043.h>
#define BRIGHTNESS_MAX 65535
#define BRIGHTNESS_STEPSIZE 256
#define FADE_DELAY 10

#define NUM_CHIPS 3
MBI5043 chip(13, 27, 32, 33, 12);
//MBI5043 chip(18, 5, 19, 6); // spi_out (SDI), spi_in (SDO), spi_clk (DCLK), spi_latch (LE), GCLK pin

uint16_t pwm_data[48]; //NUM_CHIPS*16 = 2*16 =32

//const int freqOutputPin = 12;   // OC1A output pin for ATmega32u4 (Arduino Micro)(GCLK)
//const int ocr1aval  = 0; 

void clear_data(void){
	memset(pwm_data, 0x00, sizeof(pwm_data));
}

void setup(void){
	Serial.begin(115200);
  //setCpuFrequencyMhz(40);
  //erial.println(getCpuFrequencyMhz());
  //pinMode(freqOutputPin, INPUT_PULLDOWN);
  /*  
  TCCR1A = ( (1 << COM1A0));
  TCCR1B = ((1 << WGM12) | (1 << CS10));
  TIMSK1 = 0;
  OCR1A = ocr1aval;  

  Serial.println();
  Serial.print("TCCR1A: ");Serial.println(TCCR1A,BIN);
  Serial.print("TCCR1B: ");Serial.println(TCCR1B,BIN);
  */
  clear_data();
	chip.spi_init();

	//
	// The current-set resistor 'R-EXT' of the MBI5043 should be chosen such
	// that the maximum current is only reached if the digital current gain
	// is set to '2'. That way the chip should always operate within specs.
	// 
	// The chip starts up with a gain setting of '1', which means only half
	// of the maximum current is available. Next we'll change the configuration
	// register to get a gain of '2' for full brightness.
	//
	chip.write_config(0x0000 , 0xFF, NUM_CHIPS);	// 1st number: blank configuration bits (see header file of lib), 2nd number: current gain
}



void loop(void)
{
  //for(int x = 0; x< BRIGHTNESS_MAX; x+=BRIGHTNESS_STEPSIZE){
  for(int i=0; i<48;i++){
    pwm_data[i] = BRIGHTNESS_MAX;
    chip.update(pwm_data,NUM_CHIPS);
    //Serial.println();
    //Serial.print(i+1); Serial.print(": ");Serial.println(BRIGHTNESS_MAX/8);
    delay(500);
    clear_data();
  }
    
	// Serial.print("CHIP - LED error-status: ");
	// Serial.println(chip.read_error_report(), BIN);
	// Serial.print("CHIP - config register: ");
	// Serial.println(chip.read_config(), BIN);
	// uint8_t counter;
	// uint16_t brightness;

/////////////////////////////////////////////////
	// brightness = 0;
	// while (brightness <= BRIGHTNESS_MAX) {

	// 	for (counter = 0; counter <= 15; counter++) {
	// 		pwm_data[counter] = brightness;
	// 	}

	// 	chip.update(pwm_data);


	// 	if (brightness <= BRIGHTNESS_MAX - BRIGHTNESS_STEPSIZE) {
	// 		brightness += BRIGHTNESS_STEPSIZE;
	// 	} else if (brightness == BRIGHTNESS_MAX) {
	// 		break;
	// 	} else {
	// 		brightness = BRIGHTNESS_MAX;
	// 	}
	// 	delay(FADE_DELAY);
	// }

	// while (brightness >= 0) {

	// 	for (counter = 0; counter <= 15; counter++) {
	// 		pwm_data[counter] = brightness;
	// 	}

	// 	chip.update(pwm_data);

	// 	if (brightness >= 0 + BRIGHTNESS_STEPSIZE) {
	// 		brightness -= BRIGHTNESS_STEPSIZE;
	// 	} else if (brightness == 0) {
	// 		break;
	// 	} else {
	// 		brightness = 0;
	// 	}
	// 	delay(FADE_DELAY);
	// }
	//  delay(1000);
  
}
