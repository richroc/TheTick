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

#ifndef TICK_OTA_H
#define TICK_OTA_H

#ifdef USE_OTA

#ifndef USE_WIFI
#error "USE_WIFI must be used with USE_OTA"
#endif
#ifndef USE_MDNS_RESPONDER
#error "USE_MDNS_RESPONDER must be used with USE_OTA"
#endif

#include <WiFi.h>
#include <ArduinoOTA.h>

void ota_init(void){
   // Start OTA server.
  ArduinoOTA.setPort(3232);
  ArduinoOTA.setHostname(dhcp_hostname.c_str());
  ArduinoOTA.setPassword(ota_password);

  ArduinoOTA
      .onStart([]()
               {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
    if(type == "filesystem"){
      SPIFFS.end();
    } })
      .onEnd([]()
             {
    Serial.println("\nEnd");
    ESP.restart(); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    } });

  ArduinoOTA.begin();
}

void ota_loop(void){
  ArduinoOTA.handle();
}

#else

void ota_init(void){}
void ota_loop(void){}

#endif
#endif