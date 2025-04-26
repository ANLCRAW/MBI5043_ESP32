#include "MBI5043_ESP32.h"

MBI5043::MBI5043(uint8_t spi_in_pin, uint8_t spi_clk_pin, uint8_t spi_latch_pin, uint8_t gclk_pin, uint8_t spi_out_pin, uint8_t spi_out2_pin)
{
	_spi_in_pin = spi_in_pin;
    _spi_out_pin = spi_out_pin;
    _spi_out2_pin = spi_out2_pin;
    _spi_clk_pin = spi_clk_pin;
    _spi_latch_pin = spi_latch_pin;
    _gclk_pin = gclk_pin;

    pinMode(_spi_out_pin, OUTPUT);
    if (_spi_out2_pin != 255) pinMode(_spi_out2_pin, OUTPUT);
    pinMode(_spi_clk_pin, OUTPUT);
    pinMode(_spi_latch_pin, OUTPUT);

	pinMode(_spi_in_pin, INPUT_PULLUP);

	setupGCLK(GCLK_40MHZ);
}

void MBI5043::spi_init()
{
    spi_out_low();
    if (_spi_out2_pin != 255) spi_out2_low();
    spi_clk_low();
    spi_latch_low();
}

void MBI5043::setupGCLK(GCLK_Speed speed)
{
    rmt_config_t rmt_tx = {};
    rmt_tx.rmt_mode = RMT_MODE_TX;
    rmt_tx.channel = RMT_CHANNEL_0;
    rmt_tx.gpio_num = (gpio_num_t)_gclk_pin;
    rmt_tx.mem_block_num = 1;
    rmt_tx.tx_config.loop_en = true;
    rmt_tx.tx_config.carrier_en = false;
    rmt_tx.tx_config.idle_output_en = true;
    rmt_tx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;

    // Default
    rmt_tx.clk_div = 2; // 80MHz / 2 = 40MHz base
    uint16_t pulse_ticks = 1; 

    switch (speed) {
        case GCLK_10MHZ:
            pulse_ticks = 2; // 40MHz / (2+2) = 10MHz
            break;
        case GCLK_20MHZ:
            pulse_ticks = 1; // 40MHz / (1+1) = 20MHz
            break;
        case GCLK_40MHZ:
            rmt_tx.clk_div = 1; // 80MHz / 1 = 80MHz
            pulse_ticks = 1;    // 80MHz / (1+1) = 40MHz
            break;
    }

    rmt_config(&rmt_tx);
    rmt_driver_install(RMT_CHANNEL_0, 0, 0);

    rmt_item32_t items[1] = {
        { pulse_ticks, 1, pulse_ticks, 0 }
    };

    rmt_write_items(RMT_CHANNEL_0, items, 1, true);
}

inline void MBI5043::spi_clk_high() { digitalWrite(_spi_clk_pin, HIGH); }
inline void MBI5043::spi_clk_low()  { digitalWrite(_spi_clk_pin, LOW); }
inline void MBI5043::spi_latch_high() { digitalWrite(_spi_latch_pin, HIGH); }
inline void MBI5043::spi_latch_low()  { digitalWrite(_spi_latch_pin, LOW); }
inline void MBI5043::spi_out_high() { digitalWrite(_spi_out_pin, HIGH); }
inline void MBI5043::spi_out_low()  { digitalWrite(_spi_out_pin, LOW); }
inline void MBI5043::spi_out2_high() { if (_spi_out2_pin != 255) digitalWrite(_spi_out2_pin, HIGH); }
inline void MBI5043::spi_out2_low()  { if (_spi_out2_pin != 255) digitalWrite(_spi_out2_pin, LOW); }

inline void MBI5043::pulse_spi_clk() {
    spi_clk_high();
    //delayMicroseconds(1); // Small pulse delay
    spi_clk_low();
    //delayMicroseconds(1);
}

inline void MBI5043::write_dual_data(bool bit1, bool bit2)
{
    if (bit1) spi_out_high(); else spi_out_low();
    if (_spi_out2_pin != 255) {
        if (bit2) spi_out2_high(); else spi_out2_low();
    }
}

void MBI5043::update(uint16_t* pwm_data1, uint16_t* pwm_data2, uint8_t num_chips)
{
    if (!pwm_data1 || num_chips == 0) return;

    spi_latch_low();
    uint8_t count = 0;

    for (int scan = 0; scan < 16; scan++) {
        for (uint8_t chip = 0; chip < num_chips; chip++) {
            uint16_t val1 = pwm_data1[count * num_chips + chip];
            uint16_t val2 = pwm_data2 ? pwm_data2[count * num_chips + chip] : 0;

            for (int8_t bit = 15; bit >= 0; bit--) {
                bool b1 = (val1 >> bit) & 0x01;
                bool b2 = (val2 >> bit) & 0x01;
                write_dual_data(b1, b2);
                pulse_spi_clk();
            }
        }

        spi_latch_high();
        pulse_spi_clk();
        spi_latch_low();
        count++;
    }

    spi_latch_high();
    write_dual_data(0, 0); pulse_spi_clk();
    write_dual_data(0, 0); pulse_spi_clk();
    spi_latch_low();
}

void MBI5043::write_config(uint16_t config_mask, uint16_t current_gain, uint8_t num_chips)
{
    uint16_t config_data = config_mask | current_gain;
    for (uint8_t i = 0; i < num_chips; i++) {
        for (int8_t bit = 15; bit >= 0; bit--) {
            (config_data & (1 << bit)) ? spi_out_high() : spi_out_low();
            pulse_spi_clk();
        }
    }
    spi_latch_high();
    pulse_spi_clk();
    spi_latch_low();
}

uint16_t MBI5043::read_register()
{
    uint16_t reg = 0;
    for (uint8_t i = 0; i < 16; i++) {
        reg <<= 1;
        if (digitalRead(_spi_out_pin)) reg |= 1;
        pulse_spi_clk();
    }
    return reg;
}

void MBI5043::prepare_config_read()
{
    for (int i = 0; i < 11; i++) pulse_spi_clk();
    spi_latch_high();
    for (int i = 0; i < 4; i++) pulse_spi_clk();
    spi_clk_high();
    spi_latch_low();
    spi_clk_low();
}

uint16_t MBI5043::read_config()
{
    prepare_config_read();
    return read_register();
}
