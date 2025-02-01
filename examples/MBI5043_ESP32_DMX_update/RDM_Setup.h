#ifndef _RDM_Setup_H_
#define _RDM_Setup_H_

void RDM_Setup(){
  dmx_config_t config = {
    .interrupt_flags = DMX_INTR_FLAGS_DEFAULT,
    .root_device_parameter_count = 512,
    .sub_device_parameter_count = 5,
    .model_id = 0xabcd,
    .product_category = RDM_PRODUCT_CATEGORY_FIXTURE,
    .software_version_id = 0x100,
    .software_version_label = "ANLC inc.",
    .queue_size_max = 32
  };

  dmx_personality_t personalities[] = {
    {1, "Single-channel Mode"},  // Single-address DMX personality
    {3, "RGB"},                  // Three-address RGB mode
    {4, "RGBW"},                 // Four-address RGBW personality
    {7, "RGBW with Macros"}      // RGBW with three additional macro parameters
  };
  int personality_count = 4;

  dmx_driver_install(dmxPort, &config, personalities, personality_count);

  /* Now set the DMX hardware pins to the pins that we want to use. */
  dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);

  rdm_register_identify_device(dmxPort, rdmIdentifyCallback, NULL);

  //set dmx
  rdm_register_dmx_start_address(dmxPort, rdmDMXCallback, NULL);

  //set Manufacturer label
  rdm_register_manufacturer_label(dmxPort, "ANLC inc.", rdmManufacturerLabelCallback, NULL);

  /*reset device*/
  rdm_register_reset_device(dmxPort, rdmResetCallback, NULL);
}
#endif