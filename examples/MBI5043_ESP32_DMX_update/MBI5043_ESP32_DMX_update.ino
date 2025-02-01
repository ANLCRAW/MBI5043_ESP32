
#include <MBI5043_ESP32.h>
#include <esp_dmx.h>
#include <rdm/responder.h>

#include "RDM_Globals.h"
#include "RDM_Callbacks.h"
#include "RDM_Functions.h"
#include "RDM_Setup.h"

#include "MBI_Globals.h"
#include "MBI_Setup.h"
#include "MBI_Functions.h"

void setup(void){
  Serial.begin(115200);

  MBI_Setup();
  RDM_Setup();
}

void loop(void)
{
  get_write_dmx_RDM();
}
