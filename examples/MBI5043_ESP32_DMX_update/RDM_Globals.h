#ifndef _RDM_Globals_H_
#define _RDM_Globals_H_

int transmitPin = 19;
int receivePin = 18;
int enablePin = 21;

dmx_port_t dmxPort = 1;
uint16_t DMXAddress = 1;

byte data[DMX_PACKET_SIZE];

bool identify;

bool dmxIsConnected = false;
unsigned long lastUpdate = millis();

#endif