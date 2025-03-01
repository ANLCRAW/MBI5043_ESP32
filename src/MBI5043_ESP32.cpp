
#include <stdint.h>
#include <Arduino.h>
#include "MBI5043_ESP32.h"
#include "driver/gpio.h"
#include "driver/rmt.h"
#include "soc/rmt_reg.h"


MBI5043::MBI5043(uint8_t spi_out_pin, uint8_t spi_in_pin, uint8_t spi_clk_pin, uint8_t spi_latch_pin, uint8_t gclk_pin)
{
    _spi_out_pin = spi_out_pin;
	_spi_in_PIN = spi_in_pin;
    _spi_clk_pin = spi_clk_pin;
    _spi_latch_pin = spi_latch_pin;
	
	_gclk_pin = gclk_pin;
	setupGCLK(GCLK_CLOCK_33MHZ);

	/*
    // Stel GPIO pin-modi in
    gpio_set_direction((gpio_num_t)_spi_out_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)_spi_clk_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)_spi_latch_pin, GPIO_MODE_OUTPUT);
    gpio_set_direction((gpio_num_t)_spi_in_pin, GPIO_MODE_INPUT);

    // Optioneel: pull-up/pull-down instellen indien nodig
    gpio_set_pull_mode((gpio_num_t)_spi_in_pin, GPIO_PULLUP_ONLY);

    // Maak bitmaskers voor snelle GPIO-bewerkingen
    _spi_out_pinmask = (1ULL << _spi_out_pin);
    _spi_in_pinmask = (1ULL << _spi_in_pin);
    _spi_clk_pinmask = (1ULL << _spi_clk_pin);
    _spi_latch_pinmask = (1ULL << _spi_latch_pin);
	*/

	pinMode(_spi_out_pin, OUTPUT);
    pinMode(_spi_clk_pin, OUTPUT);
    pinMode(_spi_latch_pin, OUTPUT);

	pinMode(_spi_in_PIN, INPUT_PULLUP);
}

void MBI5043::setupGCLK(uint8_t clk_speed) {
    rmt_config_t rmt_tx;
    rmt_tx.rmt_mode = RMT_MODE_TX;
    rmt_tx.channel = RMT_CHANNEL_0;
    rmt_tx.gpio_num = (gpio_num_t)_gclk_pin;
    rmt_tx.mem_block_num = 1;
    rmt_tx.clk_div = 2;  // Adjust to get ~33MHz
    rmt_tx.tx_config.loop_en = true;  // Continuous loop
    rmt_tx.tx_config.carrier_en = false;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;

    rmt_config(&rmt_tx);
    rmt_driver_install(RMT_CHANNEL_0, 0, 0);

    rmt_item32_t items[1];
    items[0].duration0 = clk_speed;  // Adjust for 33MHz
    items[0].level0 = 1;
    items[0].duration1 = clk_speed;
    items[0].level1 = 0;

    rmt_write_items(RMT_CHANNEL_0, items, 1, true);
}

void MBI5043::spi_init(void)
{
    //Zet de beginstatus van de GPIO-pinnen
    digitalWrite(_spi_out_pin, 0);
    digitalWrite(_spi_clk_pin, 0);
    digitalWrite(_spi_latch_pin, 0);

	
}

// SPI OUT HIGH
inline void MBI5043::spi_out_high(void)
{
    digitalWrite(_spi_out_pin, 1);
}

// SPI OUT LOW
inline void MBI5043::spi_out_low(void)
{
    digitalWrite(_spi_out_pin, 0);
}

// SPI CLOCK HIGH
inline void MBI5043::spi_clk_high(void)
{
    digitalWrite(_spi_clk_pin, 1);
}

// SPI CLOCK LOW
inline void MBI5043::spi_clk_low(void)
{
    digitalWrite(_spi_clk_pin, 0);
}

// LATCH HIGH
inline void MBI5043::spi_latch_high(void)
{
    digitalWrite(_spi_latch_pin, 1);
}

// LATCH LOW
inline void MBI5043::spi_latch_low(void)
{
    digitalWrite(_spi_latch_pin, 0);
}

// SPI CLOCK PULSE
inline void MBI5043::pulse_spi_clk(void)
{
    digitalWrite(_spi_clk_pin, 1);
	//delayMicroseconds(1); // removed to handle RDM @160Mhz. Works!
    digitalWrite(_spi_clk_pin, 0);
}


void MBI5043::update(uint32_t* pwm_data, uint8_t n)
{
	uint8_t data_word;
	uint8_t data_word_bit;
	uint16_t pwm_data_tmp;
	uint8_t count = 0;
	//
	// measure how long one update-cycle takes
	//
	// uint32_t start;
	// uint32_t stop;
	// start = micros();

	// send the first 15 words with "data-latch"
	// the input shift register is 16bit wide
	// it requires a "data-latch" to move the data to
	// the chip-internal buffers

	for(int i = 0; i < 16; i++){
		for (data_word = n*count; data_word < n*count + n - 1 ; data_word++) {
			pwm_data_tmp = pwm_data[data_word];
		
			for (data_word_bit = 0; data_word_bit < 16; data_word_bit++) {
				// set or clear data - MSB first !
				if (pwm_data_tmp & _BV(15)) {
					spi_out_high();
				} else {
					spi_out_low();
				}
				// pulse spi clock and shift temporary data by 1 to the left
				pulse_spi_clk();
				pwm_data_tmp <<= 1;
			}
		}
		//pulse_spi_clk();
		pwm_data_tmp = pwm_data[n*count + n - 1];
			for (data_word_bit = 0; data_word_bit < 16; data_word_bit++) {
				// set or clear data - MSB first !
				if (pwm_data_tmp & _BV(15)) {
					spi_out_high();
				} else {
					spi_out_low();
				}
				// pulse spi clock and shift temporary data by 1 to the left
				pulse_spi_clk();
				pwm_data_tmp <<= 1;
			}
			spi_latch_high();	// "data-latch" START

			if (pwm_data_tmp & _BV(15)) {
				spi_out_high();
			} else {
				spi_out_low();
			}
			pulse_spi_clk();
			//Serial.println(pwm_data_tmp,BIN);
			spi_latch_low();	// "data-latch" END
			//Serial.println("UPDATE LATCH OFF");
			count++;
	}

	// send the last word with "global-latch" 
	// to transfer the last word and make the data "live"
	count = 15;
	for (data_word = n*count; data_word < n*count + n - 1 ; data_word++) {
		pwm_data_tmp = pwm_data[data_word];
		
		for (data_word_bit = 0; data_word_bit < 15; data_word_bit++) {
				// set or clear data - MSB first !
			if (pwm_data_tmp & _BV(15)) {
				spi_out_high();
			} else {
				spi_out_low();
			}
				// pulse spi clock and shift temporary data by 1 to the left
			pulse_spi_clk();
			pwm_data_tmp <<= 1;
		}
	}
		pwm_data_tmp = pwm_data[n*count + n - 1];
			for (data_word_bit = 0; data_word_bit < 13; data_word_bit++) {
				// set or clear data - MSB first !
				if (pwm_data_tmp & _BV(15)) {
					spi_out_high();
				} else {
					spi_out_low();
				}
				// pulse spi clock and shift temporary data by 1 to the left
				pulse_spi_clk();
				pwm_data_tmp <<= 1;
			}

		if (pwm_data_tmp & _BV(15)) {
			spi_out_high();
		} else {
			spi_out_low();
		}
		pulse_spi_clk();
		pwm_data_tmp <<= 1;

		spi_latch_high();	// "global-latch" START
		if (pwm_data_tmp & _BV(15)) {
			spi_out_high();
		} else {
			spi_out_low();
		}
		pulse_spi_clk();
		pwm_data_tmp <<= 1;

		if (pwm_data_tmp & _BV(15)) {
			spi_out_high();
		} else {
			spi_out_low();
		}
		pulse_spi_clk();

		spi_latch_low();	// "global-latch" END
////////////////////////////////////////////////////////////////////////////////	
	
	//
	// measure how long one update-cycle takes
	//      
	//stop = micros();
	//Serial.println(stop-start);
}



uint16_t MBI5043::read_register(void)
{
    uint16_t register_status = 0;

    for (uint8_t register_status_bit = 0; register_status_bit < 15; register_status_bit++) {
		register_status <<= 1;

    	if (digitalRead(_spi_in_PIN)) {
            register_status |= 1;
        }
		
        pulse_spi_clk();
        
    }

	register_status <<= 1;
    // Read bit 15
    if (digitalRead(_spi_in_PIN)) {
        register_status |= 1;
    }
    return register_status;
}


void MBI5043::prepare_config_read(void)
{
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	spi_latch_high();
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	pulse_spi_clk();
	spi_clk_high();
	spi_latch_low();
	spi_clk_low();
}

uint16_t MBI5043::read_config(void)
{
	prepare_config_read();
	return read_register();
}

void MBI5043::write_config(uint16_t config_mask, uint16_t current_gain , uint8_t n) // n is no. of chips
{
	uint16_t current_gain_mask = (((uint16_t) (current_gain)));
	uint16_t config_data = (0x0000 | config_mask | current_gain_mask);
	uint8_t config_data_bit;

	/*
	Serial.println();
	Serial.print("current_gain_mask: ");
	Serial.print(current_gain_mask,BIN);
	Serial.print(" -> 0x");
	Serial.println(current_gain_mask,HEX);

	Serial.print("current_gain: ");
	Serial.print(current_gain,BIN);
	Serial.print(" -> 0x");
	Serial.println(current_gain,HEX);

	Serial.print("config_data: ");
	Serial.print(config_data,BIN);
	Serial.print(" -> 0x");
	Serial.println(config_data,HEX);
	*/
	
	// send first 5 bits
	for (config_data_bit = 0; config_data_bit <= 4; config_data_bit++) {
		if (config_data & _BV(15)) {
			spi_out_high();
		} else {
			spi_out_low();
		}
		pulse_spi_clk();
		//	Serial.print(" -> config_data: ");
		//	Serial.print(config_data,BIN);
		//	Serial.print(" -> State: ");
		//	Serial.println(config_data & _BV(15) ? "HIGH" : "LOW");
		config_data <<= 1;
	}

	spi_latch_high();
	
	// send bits 5..14
	for (config_data_bit = 5; config_data_bit <= 14; config_data_bit++) {
		if (config_data & _BV(15)) {
			spi_out_high();
		} else {
			spi_out_low();
		}
		pulse_spi_clk();
		//	Serial.print(" -> config_data: ");
		//	Serial.print(config_data,BIN);
		//	Serial.print(" -> State: ");
		//	Serial.println(config_data & _BV(15) ? "HIGH" : "LOW");
		config_data <<= 1;
	}

	// send last bit
	if (config_data & _BV(15)) {
		spi_out_high();
	} else {
		spi_out_low();
	}
	//	Serial.print(" -> config_data: ");
	//	Serial.print(config_data,BIN);
	//	Serial.print(" -> State: ");
	//	Serial.println(config_data & _BV(15) ? "HIGH" : "LOW");
	spi_clk_high();
	spi_latch_low();
	spi_clk_low();
}
/*
	Serial.println();
	Serial.print("config_mask: ");
	Serial.print(config_mask,BIN);
	Serial.print(" -> 0x");
	Serial.println(config_mask,HEX);

	Serial.print("current_gain: ");
	Serial.print(current_gain,BIN);
	Serial.print(" -> 0x");
	Serial.println(current_gain,HEX);

	
	uint16_t current_gain_mask = (((uint16_t) (current_gain)) );
	uint16_t config_data = (0x0000 | config_mask | current_gain_mask);
	uint8_t config_data_bit;
	uint16_t config_data_tmp;
	Serial.print("config_data: ");
	Serial.print(config_data,BIN);
	Serial.print(" -> 0x");
	Serial.println(config_data,HEX);

	for(int i=0; i<=n-1 ; i++){
		
		config_data_tmp = config_data;	
		for (config_data_bit = 0; config_data_bit < 16; config_data_bit++) {
			
			if (config_data_tmp & _BV(15 - config_data_bit)) {
				spi_out_high();
			} else {
				spi_out_low();
			}
			pulse_spi_clk();
				Serial.print("Puls: ");
				Serial.print(((i)*16)+config_data_bit);
				Serial.print(" -> Config_data_tmp: ");
				Serial.println(config_data_tmp,BIN);
			config_data_tmp <<= 1;
		}
		
	
		
	// send first 5 bits
	config_data_tmp = config_data;
	for (config_data_bit = 0; config_data_bit < 5; config_data_bit++) {
		if (config_data_tmp & _BV(15 - config_data_bit)) {
			spi_out_high();
		} else {
			spi_out_low();
		}
		pulse_spi_clk();
			Serial.print("5x Puls: ");
			Serial.print(config_data_bit);
			Serial.print(" -> Config_data_tmp: ");
			Serial.println(config_data_tmp,BIN);
		config_data_tmp <<= 1;
	}
	Serial.println("LATCH HIGH");
	spi_latch_high();
	// send bits 5..14
	for (config_data_bit = 5; config_data_bit < 15; config_data_bit++) {
		if (config_data_tmp & _BV(15 - config_data_bit)) {
			spi_out_high();
		} else {
			spi_out_low();
		}
		pulse_spi_clk();
			Serial.print("10x Puls: ");
			Serial.print(config_data_bit);
			Serial.print(" -> Config_data_tmp: ");
			Serial.println(config_data_tmp,BIN);
		config_data_tmp <<= 1;
	}

	// send last bit
	if (config_data_tmp & _BV(0)) {
		spi_out_high();
	} else {
		spi_out_low();
	}
		Serial.print("1x Puls: 15");
		Serial.print(" -> Config_data_tmp: ");
		Serial.println(config_data_tmp,BIN);
	spi_clk_high();
		Serial.println("CLK HIGH");
	spi_latch_low();
		Serial.println("LATCH LOW");
	spi_clk_low();
		Serial.println("CLK LOW");
}
}
*/
