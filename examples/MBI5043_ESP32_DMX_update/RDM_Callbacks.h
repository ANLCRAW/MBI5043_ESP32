#include "Print.h"
#ifndef _RDM_Callbacks_H_
#define _RDM_Callbacks_H_

#include "MBI_Functions.h"

void rdmIdentifyCallback(dmx_port_t dmxPort, rdm_header_t *request_header,
                         rdm_header_t *response_header, void *context) {
  /* We should only turn the LED on and off when we send a SET response message.
    This prevents extra work from being done when a GET request is received. */
  if (request_header->cc == RDM_CC_SET_COMMAND) {
    
    rdm_get_identify_device(dmxPort, &identify);
    //digitalWrite(ledPin, identify);
    Serial.printf("Identify mode is %s.\n", identify ? "on" : "off");
  }
}

void rdmDMXCallback(dmx_port_t dmxPort, rdm_header_t *request_header,
                      rdm_header_t * response_header, void *context){
  if (request_header->pid == RDM_PID_DMX_START_ADDRESS) {
    Serial.print("Old DMX Address = "); Serial.println(DMXAddress);
    //uint16_t dmx_address;
    rdm_get_dmx_start_address(dmxPort, &DMXAddress);
    //Serial.print("Old DMX Address = "); Serial.println(DMXAddress);
    
    rdm_set_dmx_start_address(dmxPort, DMXAddress);
    Serial.print("DMX Address = "); Serial.println(DMXAddress);
    Serial.println(request_header->pid,HEX);
    
  }
}

void rdmManufacturerLabelCallback(dmx_port_t dmxPort, rdm_header_t *request_header,
                                  rdm_header_t * response_header, void *context) {
  Serial.println("RDM Manufacturer Label Callback triggered!");
  const char *label = "ANLC inc.";
  rdm_set_device_label(dmxPort, label, strlen(label));
  Serial.print("Manufacturer Label Set: "); Serial.println(label);
}

/*reset device callback*/
void rdmResetCallback(dmx_port_t dmxPort, rdm_header_t *request_header,
                      rdm_header_t * response_header, void *context){
  if (request_header->pid == RDM_PID_RESET_DEVICE) {
    Serial.println("RESET DEVICE!");
    //Serial.println(request_header->pid, HEX);
    ESP.restart();
  }
}

#endif