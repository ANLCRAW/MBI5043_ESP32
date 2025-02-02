#ifndef _MBI_Setup_H_
#define _MBI_Setup_H_

void MBI_Setup(){
  	clear_MBI_data(); // turn off all the leds
	MBI.spi_init();

  /*
	The current-set resistor 'R-EXT' of the MBI5043 should be chosen such
	that the maximum current is only reached if the digital current gain
	is set to '2'. That way the chip should always operate within specs.
	 
	The chip starts up with a gain setting of '1', which means only half
	of the maximum current is available. Next we'll change the configuration
	register to get a gain of '2' for full brightness.
	*/
	MBI.write_config(0x0000 | CHIP_ENABLE, CURRENT_GAIN_AJUST_200, TOTAL_NUM_CHIPS);	// 1st number: blank configuration bits (see header file of lib), 2nd number: current gain
	
	

	MBI.update(pwm_data,TOTAL_NUM_CHIPS);

	Serial.print("read_config: 0x");
	Serial.println(MBI.read_config(),HEX);
}


#endif