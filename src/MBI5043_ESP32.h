#ifndef MBI5043_ESP32_H
#define MBI5043_ESP32_H

#include <Arduino.h>
#include <driver/rmt.h>

// MBI5030 configuration regiter options
// First line of each #define-block: default value

// GCLK shift
// Register bit: F~E
// - UNTESTED -
#define GCLK_SHIFT_0 0x0000
#define GCLK_SHIFT_2 0x4000
#define GCLK_SHIFT_4 0x8000
#define GCLK_SHIFT_8 0xC000

// Register bit: D - Reserved

// select pre-charge mode: Mode1, mode 2, mode 3, mode4
// Register bit: C~B
// - UNTESTED -
#define PWM_MODE_1	  0x0000
#define PWM_MODE_2    0x0800
#define PWM_MODE_3	  0x1000
#define PWM_MODE_4    0x1800

// Color shift compensation [A] (read/write)
// Register bit: A
// - WORKS -
#define SHIFT_COMPENSATION_A_OFF    0x0000
#define SHIFT_COMPENSATION_A_MODE1  0x0000
#define SHIFT_COMPENSATION_A_MODE2  0x0400
#define SHIFT_COMPENSATION_A_MODE3  0x0400

// Current gain adjustment (read/write)
// Register bit: 9~4
// - WORKS -
#define CURRENT_GAIN_ADJUST_12   0x0000
#define CURRENT_GAIN_ADJUST_50   0x0200
#define CURRENT_GAIN_ADJUST_100  0x02B0
#define CURRENT_GAIN_ADJUST_200  0x03F0

// GCLK rising/faling edge trigger
// Register bit: 3
// - UNTESTED -
#define EDGE_TRIGGER_ON  0x0008
#define EDGE_TRIGGER_OFF 0x0000


// Color shift compensation [B] (read/write)
// Register bit: 2
// - UNTESTED -
#define SHIFT_COMPENSATION_B_OFF    0x0000
#define SHIFT_COMPENSATION_B_MODE1  0x0004
#define SHIFT_COMPENSATION_B_MODE2  0x0000
#define SHIFT_COMPENSATION_B_MODE3  0x0004

// Register bit: 1 - Reserved

// Disable/Enable 
// Register bit: 0
// - UNTESTED -
#define CHIP_DISABLE  0x0000
#define CHIP_ENABLE   0x0001

enum GCLK_Speed {
    GCLK_10MHZ,
    GCLK_20MHZ,
    GCLK_40MHZ
};


class MBI5043 {
public:
    MBI5043(uint8_t spi_clk_pin, uint8_t spi_clk_pin2, uint8_t spi_latch_pin,
            uint8_t gclk_pin, uint8_t spi_out_pin, uint8_t spi_out_pin2, uint8_t spi_in_pin = 255); // default only one output

    void spi_init(int delay_ns); // num cpu cicles
    void setupGCLK(GCLK_Speed speed);
    void update(uint8_t num_chips, uint16_t* pwm_data1, uint16_t* pwm_data2);// default only one output
    void write_config(uint16_t config_data, uint8_t stripPinNr, uint8_t num_chips, uint8_t clr_pin);
    uint16_t get_preloaded_config();
    uint16_t read_config(uint8_t _clk_pin);

private:
    uint8_t _spi_in_pin;
    uint8_t _spi_clk_pin;
    uint8_t _spi_latch_pin;
    uint8_t _spi_out_pin;
    uint8_t _gclk_pin;

    uint8_t _spi_clk_pin2;
    uint8_t _spi_out_pin2;

    uint8_t _spi_delay_nops;

    uint16_t _config_data;
    uint8_t _stripPinNr;
    uint8_t _num_chips;

    void gpio_output_set(uint8_t pin);
    void gpio_output_clear(uint8_t pin);
    void gpio_output_init(uint8_t pin);
    void pulse_spi_clk(uint8_t clk_pin);
    void write_data(bool bit1, uint8_t out_pin);

    void prepare_config_read(uint8_t clk_pin);
    void prepare_config_write(uint8_t clk_pin);
    uint16_t read_register(uint8_t clk_pin);
    void write_register(uint8_t clk_pin);
};

#endif // MBI5043_ESP32_H
