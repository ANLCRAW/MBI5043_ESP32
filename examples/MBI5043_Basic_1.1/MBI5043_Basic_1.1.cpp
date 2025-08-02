#include "MBI5043_ESP32.h"

// Strip 1: SDI = 22, DCLK = 32, LE = 33, GCLK = 5 , SDO = 27
MBI5043 MBI1(23, 32, 33, 5, 22);  

// Strip 2: SDI = 18, DCLK = 25, LE = 33, GCLK = 5 , SDO = 27
MBI5043 MBI2(23, 27, 33, 5, 18);  



#define NUM_PIXELS 80  // 5 chips × 16 channels
uint16_t pwm_data1[NUM_PIXELS];
uint16_t pwm_data2[NUM_PIXELS];

void update(){
    MBI1.update(5, pwm_data1);  // 5 chips for strip 1
    MBI2.update(5, pwm_data2);  // 5 chips for strip 2
}

void clear(){
    for (int i = 0; i < NUM_PIXELS; i++) {
        pwm_data1[i] = 0x00;  // full brightness
        pwm_data2[i] = 0x00;
    }

    update();
}

void setup() {
    MBI1.spi_init(0);  // 1 µs delay
    MBI2.spi_init(0);

    MBI1.setupGCLK(GCLK_40MHZ);  // Shared GCLK, only call once
    //MBI2.setupGCLK(GCLK_20MHZ);  // Shared GCLK, only call once
    // Don't call MBI2.setupGCLK() again — it would overwrite the RMT config

    for (int i = 0; i < NUM_PIXELS; i++) {
        pwm_data1[i] = 0xFA;  // full brightness
        pwm_data2[i] = 0xFA;
    }

    update();

    delay(1000);

    clear();


}


void loop() {
    for (int i = 0; i< NUM_PIXELS; i++){
        pwm_data1[i] = 0xFFFF;
        pwm_data2[i] = 0xFFFF;
        if(i > 0){
            delay(1000);
            pwm_data1[i-1] = 0x0000;
            pwm_data2[i-1] = 0x0000;
        }
        update();
    }
    
    clear();
}