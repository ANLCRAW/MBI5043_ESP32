#include "MBI5043_ESP32.h"

MBI5043 MBI(27, 32, 33, 5, 18,22); // spi_in (SDO), spi_clk (DCLK), spi_latch (LE), GCLK pin, spi_out (SDI), spi_out2 (SDI2 | optional), --- cant use pin 35&34 for SDI&GCLK ---

#define NUM_PIXELS 80
static uint16_t pwm_data[NUM_PIXELS];
static uint16_t pwm_data1[NUM_PIXELS];

void setup() {
    MBI.spi_init();
    MBI.setupGCLK(GCLK_40MHZ);

    for (int i = 0; i < NUM_PIXELS; i++){
         pwm_data[i] = {0x00};  // Full Brightness
         pwm_data1[i] = {0x0};  // Full Brightness
    }

    MBI.update(5, pwm_data, pwm_data1); // 3 MBI5043 Chips
}

void loop() {
}