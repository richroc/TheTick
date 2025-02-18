#ifndef TICK_MDNS_RESPONDER_H
#define TICK_MDNS_RESPONDER_H

#ifdef USE_MDNS_RESPONDER

#ifndef USE_WIFI
#error "USE_MDNS_RESPONDER must be used with USE_WIFI"
#endif

#include "tick_utils.h"
#include <ESPmDNS.h>


void mdns_responder_init(void){
  if (MDNS.begin(mDNShost)) {
    output_debug_string("Open http://" + String(mDNShost) + ".local/");
  } else {
    output_debug_string("Error setting up MDNS responder!");
  }

  MDNS.addService("http", "tcp", 80);
  output_debug_string("HTTP server started\nhttp://" + String(mDNShost) + ".local/");
}

#else

void mdns_responder_init(void){}

#endif
#endif