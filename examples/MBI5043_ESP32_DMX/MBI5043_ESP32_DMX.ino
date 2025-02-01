
#include <MBI5043_ESP32.h>
#include <esp_dmx.h>
#include <rdm/responder.h>

int transmitPin = 19;
int receivePin = 18;
int enablePin = 21;

dmx_port_t dmxPort = 1;
uint16_t DMXAddress = 1;

byte data[DMX_PACKET_SIZE];

bool dmxIsConnected = false;
unsigned long lastUpdate = millis();

void rdmIdentifyCallback(dmx_port_t dmxPort, rdm_header_t *request_header,
                         rdm_header_t *response_header, void *context) {
  /* We should only turn the LED on and off when we send a SET response message.
    This prevents extra work from being done when a GET request is received. */
  if (request_header->cc == RDM_CC_SET_COMMAND) {
    bool identify;
    rdm_get_identify_device(dmxPort, &identify);
    //digitalWrite(ledPin, identify);
    Serial.printf("Identify mode is %s.\n", identify ? "on" : "off");
  }
}


//#############################################################################################################################

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

MBI5043 MBI(13, 27, 32, 33, 12); // spi_out (SDI), spi_in (SDO), spi_clk (DCLK), spi_latch (LE), GCLK pin

uint16_t pwm_data[48]; //NUM_CHIPS*16 = 2*16 =32

//Pixel order (for now 48 pixels)
uint8_t pixelArray[] = {3,0,6,12,9,15,21,18,24,30,27,33,39,36,42,1,45,4,10,7,13,19,16,22,28,25,31,37,34,40,46,43,2,8,5,11,17,14,20,26,23,29,35,32,38,44,41,47};


//const int freqOutputPin = 12;   // OC1A output pin for ATmega32u4 (Arduino Micro)(GCLK)
//const int ocr1aval  = 0; 

void clear_data(void){
	memset(pwm_data, 0x00, sizeof(pwm_data));
}

void setup(void){
  Serial.begin(115200);
  setCpuFrequencyMhz(40);
  clear_data();
	MBI.spi_init();

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

  //###############################################################
    dmx_config_t config = {
  .interrupt_flags = DMX_INTR_FLAGS_DEFAULT,
  .root_device_parameter_count = 512,
  .sub_device_parameter_count = 5,
  .model_id = 0xabcd,
  .product_category = RDM_PRODUCT_CATEGORY_FIXTURE,
  .software_version_id = 0x100,
  .software_version_label = "ANLC inc.",
  .queue_size_max = 32
};

  dmx_personality_t personalities[] = {
    {1, "Single-channel Mode"},  // Single-address DMX personality
    {3, "RGB"},                  // Three-address RGB mode
    {4, "RGBW"},                 // Four-address RGBW personality
    {7, "RGBW with Macros"}      // RGBW with three additional macro parameters
  };
  int personality_count = 4;
  dmx_driver_install(dmxPort, &config, personalities, personality_count);

  /* Now set the DMX hardware pins to the pins that we want to use. */
  dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);

  rdm_register_identify_device(dmxPort, rdmIdentifyCallback, NULL);
  //##################################################################
}

void loop(void)
{
  //###########################################################################
  dmx_packet_t packet;

  /* Now we will block until data is received.*/
  if (dmx_receive(dmxPort, &packet, DMX_TIMEOUT_TICK)) {
    /* A packet was received! If the packet was RDM, we should send a response.
      We can do this with rdm_send_response(). If the RDM packet isn't meant for
      this device, no response will be sent. */
    if (packet.is_rdm) {
      rdm_send_response(dmxPort);
      
      Serial.print("RDM request received! PID: 0x");
      Serial.println(packet.sc, HEX);
    }else {
      dmx_read(dmxPort, data, packet.size);
    }
    for(int i=0; i<TOTAL_NUM_PIXELS;i++){
      pwm_data[pixelArray[i]] = data[DMXAddress+i]* BRIGHTNESS_STEPSIZE/2;
    }
    MBI.update(pwm_data,TOTAL_NUM_CHIPS);
  }
  // Do other work here...

  //###########################################################################
  //for(int x = 0; x< BRIGHTNESS_MAX; x+=BRIGHTNESS_STEPSIZE){
  //for(int i=0; i<TOTAL_NUM_PIXELS;i++){
  //  pwm_data[pixelArray[i]] = BRIGHTNESS_MAX;
  //  MBI.update(pwm_data,TOTAL_NUM_CHIPS);
  //  //Serial.println();
  //  //Serial.print(i+1); Serial.print(": ");Serial.println(BRIGHTNESS_MAX/8);
  //  delay(500);
  //  clear_data();
  //}
    
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
