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

#include <ArduinoOTA.h>
#include <WiFi.h>

#include "tick_utils.h"

static void ota_handler_start(void) {
  String type;
  if (ArduinoOTA.getCommand() == U_FLASH) {
    type = "sketch";
  } else {  // U_SPIFFS
    type = "filesystem";
    SPIFFS.end();
  }

  DBG_OUTPUT_PORT.println("OTA Start updating " + type);
}

static void ota_handler_end(void) {
  DBG_OUTPUT_PORT.println("\nOTA End");
  ESP.restart();
}

static void ota_handler_error(ota_error_t error) {
  DBG_OUTPUT_PORT.printf("Error[%u]: ", error);
  if (error == OTA_AUTH_ERROR) {
    DBG_OUTPUT_PORT.println("OTA Auth Failed");
  } else if (error == OTA_BEGIN_ERROR) {
    DBG_OUTPUT_PORT.println("OTA Begin Failed");
  } else if (error == OTA_CONNECT_ERROR) {
    DBG_OUTPUT_PORT.println("OTA Connect Failed");
  } else if (error == OTA_RECEIVE_ERROR) {
    DBG_OUTPUT_PORT.println("OTA Receive Failed");
  } else if (error == OTA_END_ERROR) {
    DBG_OUTPUT_PORT.println("OTA End Failed");
  }
}

void ota_init(void) {
  ArduinoOTA.setPort(3232);
  ArduinoOTA.setHostname(dhcp_hostname.c_str());
  ArduinoOTA.setPassword(ota_password);

  ArduinoOTA.onStart(ota_handler_start)
      .onEnd(ota_handler_end)
      .onError(ota_handler_error);

  ArduinoOTA.begin();
}

void ota_loop(void) { ArduinoOTA.handle(); }

#else

void ota_init(void) {}
void ota_loop(void) {}

#endif
#endif