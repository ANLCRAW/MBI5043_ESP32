#ifndef MBI5043_ESP32_H
#define MBI5043_ESP32_H

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
#define CURRENT_GAIN_ADJUST_100  0x2B0
#define CURRENT_GAIN_ADJUST_12   0x000
#define CURRENT_GAIN_ADJUST_200  0x3F0


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

#include <Arduino.h>
#include <driver/rmt.h>

extern "C" {
#include <stdint.h>
}

class MBI5043 {
public:
    enum GCLK_Speed {
        GCLK_10MHZ,
        GCLK_20MHZ,
        GCLK_40MHZ
    };

    MBI5043(uint8_t spi_in_pin, uint8_t spi_clk_pin, uint8_t spi_latch_pin,
            uint8_t gclk_pin, uint8_t spi_out_pin, uint8_t spi_out2_pin = 255);

    void spi_init();
    void update(uint16_t* pwm_data1, uint16_t* pwm_data2, uint8_t num_chips);
    void write_config(uint16_t config_mask, uint16_t current_gain, uint8_t num_chips);
    uint16_t read_config();

private:
    uint8_t _spi_in_pin;
    uint8_t _spi_clk_pin;
    uint8_t _spi_latch_pin;
    uint8_t _spi_out_pin;
    uint8_t _spi_out2_pin;
    uint8_t _gclk_pin;

    void setupGCLK(GCLK_Speed speed);
    void gpio_output_set(uint8_t pin);
    void gpio_output_clear(uint8_t pin);
    void gpio_output_init(uint8_t pin);
    void pulse_spi_clk();
    void write_dual_data(bool bit1, bool bit2);
    void prepare_config_read();
    uint16_t read_register();
};

#endif // MBI5043_ESP32_H
