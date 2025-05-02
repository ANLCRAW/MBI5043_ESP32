#include "MBI5043_ESP32.h"
#include "driver/rmt.h"
#include "soc/gpio_struct.h"
#include "soc/io_mux_reg.h"
#include "driver/gpio.h"
#include "esp32/rom/gpio.h"

MBI5043::MBI5043(uint8_t spi_in_pin, uint8_t spi_clk_pin, uint8_t spi_latch_pin,
                 uint8_t gclk_pin, uint8_t spi_out_pin, uint8_t spi_out2_pin)
{
    _spi_in_pin = spi_in_pin;
    _spi_clk_pin = spi_clk_pin;
    _spi_latch_pin = spi_latch_pin;
    _gclk_pin = gclk_pin;
    _spi_out_pin = spi_out_pin;
    _spi_out2_pin = spi_out2_pin;

    gpio_output_init(_spi_out_pin);
    if (_spi_out2_pin != 255) gpio_output_init(_spi_out2_pin);
    gpio_output_init(_spi_clk_pin);
    gpio_output_init(_spi_latch_pin);

    gpio_pad_select_gpio((gpio_num_t)_spi_in_pin);
    gpio_set_direction((gpio_num_t)_spi_in_pin, GPIO_MODE_INPUT);
    gpio_set_pull_mode((gpio_num_t)_spi_in_pin, GPIO_PULLUP_ONLY);

}

void MBI5043::spi_init(uint8_t delay_us)
{
    _spi_delay_us = delay_us;

    gpio_output_clear(_spi_out_pin);
    if (_spi_out2_pin != 255) gpio_output_clear(_spi_out2_pin);
    gpio_output_clear(_spi_clk_pin);
    gpio_output_clear(_spi_latch_pin);
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

    rmt_tx.clk_div = (speed == GCLK_40MHZ) ? 1 : 2;
    uint16_t pulse_ticks = (speed == GCLK_10MHZ) ? 2 : 1;

    rmt_config(&rmt_tx);
    rmt_driver_install(RMT_CHANNEL_0, 0, 0);

    rmt_item32_t items[1] = { { pulse_ticks, 1, pulse_ticks, 0 } };
    rmt_write_items(RMT_CHANNEL_0, items, 1, true);
}

void MBI5043::gpio_output_init(uint8_t pin)
{
    gpio_pad_select_gpio((gpio_num_t)pin);
    gpio_set_direction((gpio_num_t)pin, GPIO_MODE_OUTPUT);
}

void MBI5043::gpio_output_set(uint8_t pin)
{
    if (pin < 32)
        GPIO.out_w1ts = (1UL << pin);
    else
        GPIO.out1_w1ts.val = (1UL << (pin - 32));
}

void MBI5043::gpio_output_clear(uint8_t pin)
{
    if (pin < 32)
        GPIO.out_w1tc = (1UL << pin);
    else
        GPIO.out1_w1tc.val = (1UL << (pin - 32));
}


void MBI5043::pulse_spi_clk()
{
    gpio_output_set(_spi_clk_pin);
    if (_spi_delay_us > 0) delayMicroseconds(_spi_delay_us);
    gpio_output_clear(_spi_clk_pin);
    if (_spi_delay_us > 0) delayMicroseconds(_spi_delay_us);
}

void MBI5043::write_dual_data(bool bit1, bool bit2)
{
    if (bit1) gpio_output_set(_spi_out_pin); else gpio_output_clear(_spi_out_pin);
    if (_spi_out2_pin != 255)
        if (bit2) gpio_output_set(_spi_out2_pin); else gpio_output_clear(_spi_out2_pin);

    if (_spi_delay_us > 0) delayMicroseconds(_spi_delay_us/2);
}

void MBI5043::update(uint8_t num_chips, uint16_t* pwm_data1, uint16_t* pwm_data2)
{
    if (!pwm_data1 || num_chips == 0) return;

    gpio_output_clear(_spi_latch_pin);
    uint8_t count = 0;

    for (int scan = 0; scan < 16; scan++) {
        for (uint8_t chip = 0; chip < num_chips; chip++) {
            uint16_t val1 = pwm_data1[count * num_chips + chip];
            uint16_t val2 = pwm_data2 ? pwm_data2[count * num_chips + chip] : 0;

            for (int8_t bit = 15; bit >= 0; bit--) {
                write_dual_data((val1 >> bit) & 1, (val2 >> bit) & 1);
                pulse_spi_clk();
            }
        }

        gpio_output_set(_spi_latch_pin);
        pulse_spi_clk();
        gpio_output_clear(_spi_latch_pin);
        count++;
    }

    gpio_output_set(_spi_latch_pin);
    if (_spi_delay_us > 0) delayMicroseconds(_spi_delay_us);
    write_dual_data(0, 0); pulse_spi_clk();
    write_dual_data(0, 0); pulse_spi_clk();
    gpio_output_clear(_spi_latch_pin);
}


void MBI5043::write_register()
{
    // Shift out data for each chip (start with furthest)
    for (int8_t chip = _num_chips - 1; chip >= 0; chip--) {
        for (int8_t bit = 15; bit >= 0; bit--) {
            if (chip == 0 && bit == 10) {
                // LE goes HIGH before final 11 bits of last chip
                gpio_output_set(_spi_latch_pin);
            }

            if (_config_data & (1 << bit))
                gpio_output_set(_stripPinNr);
            else
                gpio_output_clear(_stripPinNr);

            pulse_spi_clk();
        }
    // LE LOW after all bits sent
    gpio_output_clear(_spi_latch_pin);
    }
}

void MBI5043::prepare_config_write()
{   
    pulse_spi_clk();
    gpio_output_set(_spi_latch_pin);
    for (int i = 0; i < 15; i++) pulse_spi_clk();
    gpio_output_clear(_spi_latch_pin);
   
}

void MBI5043::write_config(uint16_t config_data, uint8_t stripPinNr, uint8_t num_chips)
{
    _config_data = config_data;
    _stripPinNr = stripPinNr;
    _num_chips = num_chips;

    prepare_config_write();
    write_register();
}

uint16_t MBI5043::get_preloaded_config(){
    return _config_data;
}

uint16_t MBI5043::read_register()
{
    uint16_t reg = 0;
    for (uint8_t i = 0; i < 16; i++) {
        reg <<= 1;
        if (gpio_get_level((gpio_num_t)_spi_in_pin)) reg |= 1;
        pulse_spi_clk();
    }
    return reg;
}

void MBI5043::prepare_config_read()
{
    for (int i = 0; i < 11; i++) pulse_spi_clk();
    gpio_output_set(_spi_latch_pin);
    for (int i = 0; i < 4; i++) pulse_spi_clk();
    gpio_output_set(_spi_clk_pin);
    gpio_output_clear(_spi_latch_pin);
    gpio_output_clear(_spi_clk_pin);
}

uint16_t MBI5043::read_config()
{
    prepare_config_read();
    return read_register();
}
