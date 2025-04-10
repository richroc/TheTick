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

#ifndef TICK_CONFIG_HANDLING_H
#define TICK_CONFIG_HANDLING_H

#include <SPIFFSIniFile.h>

#include "tick_utils.h"

bool loadConfig() {
  const size_t bufferLen = 80;
  char buffer[bufferLen];

  output_debug_string("Reading config START");

  SPIFFSIniFile ini(CONFIG_FILE);
  if (!ini.open()) {
    output_debug_string("Config file not present");
    return false;
  }

  if (!ini.validate(buffer, bufferLen)) {
    ini.close();
    output_debug_string("Config file invalid");
    return false;
  }

  char t_tick_mode[CONFIG_VAR_LENGTH];
  ini.getValue("tick", "mode", buffer, bufferLen, t_tick_mode,
               CONFIG_VAR_LENGTH);

#ifdef USE_WIEGAND
  if (strcasecmp(t_tick_mode, "WIEGAND") == 0) {
    current_tick_mode = tick_mode_wiegand;
  }
  ini.getValue("wiegand", "dos_id", buffer, bufferLen, DoS_id,
               CONFIG_PASSWORD_LENGTH);
  ini.getValue("wiegand", "pin_d0", buffer, bufferLen, wiegand_pin_d0);
  ini.getValue("wiegand", "pin_d1", buffer, bufferLen, wiegand_pin_d1);
  ini.getValue("wiegand", "pulse_width", buffer, bufferLen,
               wiegand_pulse_width);
  ini.getValue("wiegand", "pulse_gap", buffer, bufferLen, wiegand_pulse_gap);
#endif

#ifdef USE_CLOCKANDDATA
  if (strcasecmp(t_tick_mode, "CLOCKANDDATA") == 0) {
    current_tick_mode = tick_mode_clockanddata;
  }
  ini.getValue("clockanddata", "pin_clock", buffer, bufferLen,
               clockanddata_pin_clock);
  ini.getValue("clockanddata", "pin_data", buffer, bufferLen,
               clockanddata_pin_data);
  ini.getValue("clockanddata", "pulse_width", buffer, bufferLen,
               clockanddata_pulse_width);
#endif
#ifdef USE_OSDP
if (strcasecmp(t_tick_mode, "OSDP_PD") == 0) {
  current_tick_mode = tick_mode_osdp_pd;

}
if (strcasecmp(t_tick_mode, "OSDP_CP") == 0) {
  current_tick_mode = tick_mode_osdp_cp;
}

ini.getValue("osdp", "baudrate", buffer, bufferLen, osdp_baudrate);

if (osdp_baudrate != 9600 && osdp_baudrate != 19200 && osdp_baudrate != 38400 &&
    osdp_baudrate != 115200 && osdp_baudrate != 230400) {
  output_debug_string("Invalid OSDP baudrate, using default");
  osdp_baudrate = OSDP_BAUDATE;
}

ini.getValue("osdp", "terminator", buffer, bufferLen, osdp_terminator);
ini.getValue("osdp", "address", buffer, bufferLen, osdp_address);

ini.getValue("osdp", "scbk", buffer, bufferLen, osdp_pin_term);
ini.getValue("osdp", "mk", buffer, bufferLen, osdp_pin_term);



#endif

#ifdef USE_WIFI
  ini.getValue("wifi_hotspot", "enable", buffer, bufferLen, ap_enable);
  ini.getValue("wifi_hotspot", "hidden", buffer, bufferLen, ap_hidden);
  ini.getValue("wifi_hotspot", "ssid", buffer, bufferLen, ap_ssid,
               CONFIG_SSID_LENGTH);
  ini.getValue("wifi_hotspot", "psk", buffer, bufferLen, ap_psk,
               CONFIG_PASSWORD_LENGTH);

  ini.getValue("wifi_client", "ssid", buffer, bufferLen, station_ssid,
               CONFIG_SSID_LENGTH);
  ini.getValue("wifi_client", "psk", buffer, bufferLen, station_psk,
               CONFIG_PASSWORD_LENGTH);
#endif

#ifdef USE_MDNS_RESPONDER
  ini.getValue("mdns", "host", buffer, bufferLen, mDNShost, CONFIG_VAR_LENGTH);
#endif

#ifdef USE_SYSLOG
  ini.getIPAddress("syslog", "server", buffer, bufferLen, syslog_server);
  ini.getValue("syslog", "port", buffer, bufferLen, syslog_port);
  ini.getValue("syslog", "priority", buffer, bufferLen, syslog_priority);
  ini.getValue("syslog", "service", buffer, bufferLen, syslog_service_name,
               CONFIG_VAR_LENGTH);
  ini.getValue("syslog", "host", buffer, bufferLen, syslog_host,
               CONFIG_VAR_LENGTH);
#endif

#ifdef USE_OTA
  ini.getValue("ota", "password", buffer, bufferLen, ota_password,
               CONFIG_PASSWORD_LENGTH);
#endif

#ifdef USE_HTTP
  ini.getValue("http", "username", buffer, bufferLen, www_username,
               CONFIG_VAR_LENGTH);
  ini.getValue("http", "password", buffer, bufferLen, www_password,
               CONFIG_PASSWORD_LENGTH);
#endif

#ifdef USE_BLE
  ini.getValue("ble", "service", buffer, bufferLen, ble_uuid_wiegand_service,
               CONFIG_UUID_LENGTH);
  ini.getValue("ble", "characteristic", buffer, bufferLen,
               ble_uuid_wiegand_characteristic, CONFIG_UUID_LENGTH);
  ini.getValue("ble", "passkey", buffer, bufferLen, ble_passkey);
#endif

  ini.close();

  output_debug_string("Reading config END");
  return true;
}

#endif