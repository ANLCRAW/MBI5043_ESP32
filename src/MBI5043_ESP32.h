//
// To get a suitably high GSCLK frequency, the CLKO-FUSE of the ATMega168/328 has been programmed
// It will output it's system clock on PB0 ("digital pin" #8)
//
// THIS REQUIRES AN ISP PROGRAMMER (or 2nd Arduino loaded with Arduino-ISP)
//
// Diecimila + ATmega168: lfuse: 0xBF
//                        hfuse: 0xDD
//                        efuse: 0x00 (or 0xF8)
//
// Uno + ATmega328:       lfuse: 0xBF
//                        hfuse: 0xDE
//                        efuse: 0x05 (or 0xFD)
//                        
// To revert to Arduino's default FUSE settings, change 0xBF to 0xFF.
//
// Use: "http://www.engbedded.com/fusecalc" and ".../arduino-XXX/hardware/arduino/boards.txt"
//
// Make sure to get the latest MBI5030 datasheet (at least version Jan. 2009)!
// 

#ifndef _MBI5043_H_
#define _MBI5043_H_



// MBI5030 configuration regiter options
// First line of each #define-block: default value

// GCLK shift
// Register bit: F~E
// - UNTESTED -
#define GCLK_SHIFT_0 0x0000
#define GCLK_SHIFT_2 0x4000
#define GCLK_SHIFT_4 0x8000
#define GCLK_SHIFT_8 0xC000

// PWM resolution (read/write)
// Register bit: D
// - WORKS -
#define PWM_16BIT 0x0000
#define PWM_12BIT 0x2000

// select pre-charge mode: Mode1, mode 2, mode 3, mode4
// Register bit: C~B
// - WORKS - 
#define PWM_MODE_1	  0x0000
#define PWM_MODE_2    0x0800
#define PWM_MODE_3	  0x1000
#define PWM_MODE_4    0x1800

// Current gain adjustment (read/write)
// Register bit: 9~4
// - UNTESTED -
#define CURRENT_GAIN_AJUST_100  0x2B0
#define CURRENT_GAIN_AJUST_12   0x0000
#define CURRENT_GAIN_AJUST_200  0x3F0


// Color shift compensation [A] (read/write)
// Register bit: A
// - UNTESTED -
#define SHIFT_COMPENSATION_A_OFF    0x0000
#define SHIFT_COMPENSATION_A_MODE1  0x0000
#define SHIFT_COMPENSATION_A_MODE2  0x0400
#define SHIFT_COMPENSATION_A_MODE3  0x0400

// Color shift compensation [B] (read/write)
// Register bit: 2
// - UNTESTED -
#define SHIFT_COMPENSATION_B_OFF    0x0000
#define SHIFT_COMPENSATION_B_MODE1  0x0004
#define SHIFT_COMPENSATION_B_MODE2  0x0000
#define SHIFT_COMPENSATION_B_MODE3  0x0004

// GCLK rising/faling edge trigger
// Register bit: 3
// - UNTESTED -
#define EDGE_TRIGGER_ON  0x0008
#define EDGE_TRIGGER_OFF 0x0000

// Disable/Enable 
// Register bit: 0
// - UNTESTED -
#define CHIP_DISABLE  0x0000
#define CHIP_ENABLE   0x0001

extern "C" {
#include <stdint.h>
}

class MBI5043 {
 public:
	MBI5043(uint8_t spi_out_pin, uint8_t spi_in_pin, uint8_t spi_clk_pin,
		uint8_t spi_latch_pin, uint8_t gclk_pin);
	void spi_init(void);
	void update(uint16_t * pwm_data, uint8_t n);
	uint16_t read_config(void);
	void write_config(uint16_t config_mask, uint16_t current_gain, uint8_t n);

 private:
	volatile uint8_t *_spi_out_DIR;
	volatile uint8_t *_spi_out_PORT;
	uint8_t _spi_out_pinmask;

	volatile uint8_t *_spi_in_DIR;
	volatile uint8_t *_spi_in_PIN;
	uint8_t _spi_in_pinmask;

	volatile uint8_t *_spi_clk_DIR;
	volatile uint8_t *_spi_clk_PORT;
	uint8_t _spi_clk_pinmask;

	volatile uint8_t *_spi_latch_DIR;
	volatile uint8_t *_spi_latch_PORT;
	uint8_t _spi_latch_pinmask;

	uint8_t _spi_out_pin;
	uint8_t _spi_in_pin;
	uint8_t _spi_clk_pin;
	uint8_t _spi_latch_pin;
	uint8_t _gclk_pin;

	inline void spi_clk_high(void) __attribute__ ((always_inline));
	inline void spi_clk_low(void) __attribute__ ((always_inline));
	inline void spi_out_high(void) __attribute__ ((always_inline));
	inline void spi_out_low(void) __attribute__ ((always_inline));
	inline void spi_latch_high(void) __attribute__ ((always_inline));
	inline void spi_latch_low(void) __attribute__ ((always_inline));
	inline void pulse_spi_clk(void) __attribute__ ((always_inline));
	void setupGCLK(void);

	uint16_t read_register(void);
	void prepare_config_read(void);
};

#endif
