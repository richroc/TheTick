// Copyright (C) 2024, 2025 Jakub "lenwe" Kramarz
// This file is part of The Tick.
// 
// The Tick is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// The Tick is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License

#ifdef USE_WIFI
#include "tick_wifi.h"
#include "tick_utils.h"
#include <WiFi.h>

void wifi_init(void){
  output_debug_string("WIFI INIT STARTED");
  WiFi.hostname(dhcp_hostname);

  // Check WiFi connection
  // ... check mode
  if (WiFi.getMode() != WIFI_STA) {
    WiFi.mode(WIFI_STA);
    delay(10);
  }

  // ... Compare file config with sdk config.
  if (WiFi.SSID() != station_ssid || WiFi.psk() != station_psk) {
    output_debug_string(F("WiFi config changed."));

    // ... Try to connect as WiFi station.
    WiFi.begin(station_ssid, station_psk);

    output_debug_string("new SSID: " + String(WiFi.SSID()));

    // ... Uncomment this for debugging output.
    WiFi.printDiag(Serial);
  } else {
    // ... Begin with sdk config.
    WiFi.begin();
  }
  output_debug_string(F("Wait for WiFi connection."));

  if(strlen(station_ssid) > 0){

    // ... Give ESP 10 seconds to connect to station.
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 60000) {
      DBG_OUTPUT_PORT.write('.');
      DBG_OUTPUT_PORT.print(WiFi.status());
      delay(500);
    }
    DBG_OUTPUT_PORT.println();
  }

  // Check connection
  if (WiFi.status() == WL_CONNECTED) {
    // ... print IP Address
    DBG_OUTPUT_PORT.print("IP address: ");
    output_debug_string(WiFi.localIP().toString());
  } else {
    if (ap_enable) {
      output_debug_string(F("Going into AP mode."));

      // Go into software AP mode.
      WiFi.mode(WIFI_AP);
      WiFi.softAPConfig(ap_ip, ap_ip, IPAddress(255, 255, 255, 0));
      WiFi.softAP(ap_ssid, ap_psk, 0, ap_hidden);

      output_debug_string(
        "SSID: " + String(ap_ssid) + "\n" +
        "PSK: " + String(ap_psk) + "\n" +
        "IP: " + WiFi.softAPIP().toString()
      );
    } else {
      output_debug_string("Can not connect to WiFi station. Bummer.");
      WiFi.mode(WIFI_OFF);
    }
  }

}

#else

void wifi_init(void){}

#endif