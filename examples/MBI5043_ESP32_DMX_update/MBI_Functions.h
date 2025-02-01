#ifndef _MBI_Functions_H_
#define _MBI_Functions_H_

#include "MBI_Globals.h"

  void clear_MBI_data(void){
    memset(pwm_data, 0x00, sizeof(pwm_data));
  }

  void write_pixels(){
    for(int i=0; i<TOTAL_NUM_PIXELS;i++){
      pwm_data[pixelArray[i]] = data[DMXAddress+i]* BRIGHTNESS_STEPSIZE/2;
    }
    MBI.update(pwm_data,TOTAL_NUM_CHIPS);
  }

  void MBI_indentify(){
    // RDM command for identification was sent.
    // Blink the device.
    unsigned long now = millis();
    if (now % 1000 < 250) {
      for(int i=0; i<TOTAL_NUM_PIXELS; i++) {  
        pwm_data[pixelArray[i]]= 255*BRIGHTNESS_STEPSIZE/2;
      }
    } else {
      clear_MBI_data();
    }
    MBI.update(pwm_data,TOTAL_NUM_CHIPS);
  }

#endif