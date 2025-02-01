#ifndef _RDM_Functions_H_
#define _RMD_Functions_H_

void get_write_dmx_RDM(){
  dmx_packet_t packet;

  /* Now we will block until data is received.*/
  if (dmx_receive(dmxPort, &packet, DMX_TIMEOUT_TICK)) {
    /* A packet was received! If the packet was RDM, we should send a response.
      We can do this with rdm_send_response(). If the RDM packet isn't meant for
      this device, no response will be sent. */
    if (packet.is_rdm) {
      rdm_send_response(dmxPort);
      
      Serial.print("RDM request received! PID: 0x");
      Serial.println(packet.sc, HEX);
    }else if (identify){
      MBI_indentify();
    }else{
      dmx_read(dmxPort, data, packet.size);
      write_pixels();
    }
    
  }
  // Do other work here...
}

#endif