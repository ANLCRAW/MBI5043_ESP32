#include "MBI5043_ESP32.h"

MBI5043 MBI(27, 32, 33, 5, 18); // spi_in (SDO), spi_clk (DCLK), spi_latch (LE), GCLK pin, spi_out (SDI), spi_out2 (SDI2 | optional), --- cant use pin 35&34 for SDI&GCLK ---

void setup() {
    MBI.spi_init();
    MBI.setupGCLK(GCLK_40MHZ);

    static uint16_t pwm_data[48] = {0xFFFF};  // Full Brightness

    MBI.update(3, pwm_data); // 3 MBI5043 Chips
}

void loop() {
}