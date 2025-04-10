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

#include <Arduino.h>
#include <IPAddress.h>
#include <SPIFFSIniFile.h>
#include <cstddef>
#include "HardwareSerial.h"
#include "tick_default_config.h"
#include "tick_utils.h"

char log_name[CONFIG_VAR_LENGTH];

enum tick_mode current_tick_mode;

#ifdef USE_WIEGAND
int wiegand_pin_d0 = PIN_D0;
int wiegand_pin_d1 = PIN_D1;
int wiegand_pulse_width = WIEGAND_PULSE_WIDTH;
int wiegand_pulse_gap = WIEGAND_PULSE_GAP;
#endif

#ifdef USE_CLOCKANDDATA
int clockanddata_pin_clock = PIN_D0;
int clockanddata_pin_data = PIN_D1;
int clockanddata_pulse_width = CLOCKANDDATA_PULSE_WIDTH;
#endif

#ifdef USE_OSDP
int osdp_pin_term;
int osdp_pin_de;
int osdp_pin_re;
int osdp_pin_rx;
int osdp_pin_tx;
bool osdp_terminator;
HardwareSerial *osdp_serial;

int osdp_baudrate;
int osdp_address;

char osdp_scbk[CONFIG_PASSWORD_LENGTH];
char osdp_mk[CONFIG_PASSWORD_LENGTH];
#endif

#ifdef USE_BLE
char ble_uuid_wiegand_service[CONFIG_UUID_LENGTH];
char ble_uuid_wiegand_characteristic[CONFIG_UUID_LENGTH];
uint32_t ble_passkey;
#endif

#ifdef USE_WIFI
bool ap_enable;
bool ap_hidden;
char ap_ssid[CONFIG_SSID_LENGTH];
IPAddress ap_ip(192, 168, 4, 1);
char ap_psk[CONFIG_PASSWORD_LENGTH];
char station_ssid[CONFIG_SSID_LENGTH];
char station_psk[CONFIG_PASSWORD_LENGTH];
#endif

#ifdef USE_MDNS_RESPONDER
char mDNShost[CONFIG_VAR_LENGTH];
#endif

char DoS_id[CONFIG_PASSWORD_LENGTH];

#ifdef USE_OTA
char ota_password[CONFIG_PASSWORD_LENGTH];
#endif

#ifdef USE_HTTP
char www_username[CONFIG_VAR_LENGTH];
char www_password[CONFIG_PASSWORD_LENGTH];
#endif

#ifdef USE_SYSLOG
IPAddress syslog_server;
unsigned int syslog_port;
char syslog_service_name[CONFIG_VAR_LENGTH];
char syslog_host[CONFIG_VAR_LENGTH];
byte syslog_priority;
#endif

void set_default_config(void) {
  strcpy(log_name, LOG_NAME);

  current_tick_mode = tick_mode_disabled;

#ifdef USE_WIEGAND
  wiegand_pin_d0 = PIN_D0;
  wiegand_pin_d1 = PIN_D1;
  wiegand_pulse_width = WIEGAND_PULSE_WIDTH;
  wiegand_pulse_gap = WIEGAND_PULSE_GAP;
#endif

#ifdef USE_CLOCKANDDATA
  clockanddata_pin_clock = PIN_D0;
  clockanddata_pin_data = PIN_D1;
  clockanddata_pulse_width = CLOCKANDDATA_PULSE_WIDTH;
#endif

#ifdef USE_OSDP
  osdp_pin_term = PIN_OSDP_TERM;
  osdp_pin_de = PIN_OSDP_DE;
  osdp_pin_re = PIN_OSDP_RE;
  osdp_pin_rx = PIN_OSDP_RX;
  osdp_pin_tx = PIN_OSDP_TX;
  osdp_terminator = false;
  osdp_serial = &Serial1;

  osdp_baudrate = OSDP_BAUDRATE;
  osdp_address = 101;

  strcpy(osdp_scbk, "");
  strcpy(osdp_mk, "");
#endif

#ifdef USE_BLE
  strcpy(ble_uuid_wiegand_service, BLE_UUID_WIEGAND_SERVICE);
  strcpy(ble_uuid_wiegand_characteristic, BLE_UUID_WIEGAND_CHARACTERISTIC);
  ble_passkey = BLE_PASSKEY;
#endif

#ifdef USE_WIFI
  ap_enable = AP_ENABLE;
  ap_hidden = AP_HIDDEN;
  strcpy(ap_ssid, AP_SSID);
  ap_ip.fromString(AP_IP);

  strcpy(ap_psk, AP_PSK);  // Default PSK.
  strcpy(station_ssid, STATION_SSID);
  strcpy(station_psk, STATION_PSK);
#endif

#ifdef USE_MDNS_RESPONDER
// strcpy(mDNShost, MDNSSERVICE);
#endif

  strcpy(DoS_id, DOS_CARD_ID);

#ifdef USE_OTA
  strcpy(ota_password, OTA_PASSWORD);
#endif

#ifdef USE_HTTP
  strcpy(www_username, WWW_USERNAME);
  strcpy(www_password, WWW_PASSWORD);
#endif

#ifdef USE_SYSLOG
  syslog_server.fromString(SYSLOG_SERVER);
  syslog_port = SYSLOG_PORT;
  strcpy(syslog_service_name, SYSLOG_SERVICE);
  strcpy(syslog_host, SYSLOG_HOSTNAME);
  syslog_priority = SYSLOG_PRIORITY;
#endif
}

bool loadConfig() {
  set_default_config();
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

  if (osdp_baudrate != 9600 && osdp_baudrate != 19200 &&
      osdp_baudrate != 38400 && osdp_baudrate != 115200 &&
      osdp_baudrate != 230400) {
    output_debug_string("Invalid OSDP baudrate, using default");
    osdp_baudrate = OSDP_BAUDRATE;
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
