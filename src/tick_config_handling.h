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
  
  #ifdef USE_WIEGAND
  ini.getValue("wiegand", "dos_id", buffer, bufferLen, DoS_id, CONFIG_PASSWORD_LENGTH);
  ini.getValue("wiegand", "pin_d0", buffer, bufferLen, wiegand_pin_d0);
  ini.getValue("wiegand", "pin_d1", buffer, bufferLen, wiegand_pin_d1);
  ini.getValue("wiegand", "pulse_width", buffer, bufferLen, wiegand_pulse_width);
  ini.getValue("wiegand", "pulse_gap", buffer, bufferLen, wiegand_pulse_gap);
  #endif

  #ifdef USE_WIFI
  ini.getValue("wifi_hotspot", "enable", buffer, bufferLen, ap_enable);
  ini.getValue("wifi_hotspot", "hidden", buffer, bufferLen, ap_hidden);
  ini.getValue("wifi_hotspot", "ssid", buffer, bufferLen, ap_ssid, CONFIG_VAR_LENGTH);
  ini.getValue("wifi_hotspot", "psk", buffer, bufferLen, ap_psk, CONFIG_PASSWORD_LENGTH);

  ini.getValue("wifi_client", "ssid", buffer, bufferLen, station_ssid, CONFIG_VAR_LENGTH);
  ini.getValue("wifi_client", "psk", buffer, bufferLen, station_psk, CONFIG_PASSWORD_LENGTH);
  #endif

  #ifdef USE_MDNS_RESPONDER
  ini.getValue("mdns", "host", buffer, bufferLen, mDNShost, CONFIG_VAR_LENGTH);
  #endif

  #ifdef USE_SYSLOG
  ini.getIPAddress("syslog", "server", buffer, bufferLen, syslog_server);
  ini.getValue("syslog", "port", buffer, bufferLen, syslog_port);
  ini.getValue("syslog", "priority", buffer, bufferLen, syslog_priority);
  ini.getValue("syslog", "service", buffer, bufferLen, syslog_service_name, CONFIG_VAR_LENGTH);
  ini.getValue("syslog", "host", buffer, bufferLen, syslog_host, CONFIG_VAR_LENGTH);
  #endif

  #ifdef USE_OTA
  ini.getValue("ota", "password", buffer, bufferLen, ota_password, CONFIG_PASSWORD_LENGTH);
  #endif

  #ifdef USE_HTTP
  ini.getValue("http", "username", buffer, bufferLen, www_username, CONFIG_VAR_LENGTH);
  ini.getValue("http", "password", buffer, bufferLen, www_password, CONFIG_PASSWORD_LENGTH);
  #endif

  #ifdef USE_BLE
  ini.getValue("ble", "service", buffer, bufferLen, ble_uuid_wiegand_service, CONFIG_UUID_LENGTH);
  ini.getValue("ble", "characteristic", buffer, bufferLen, ble_uuid_wiegand_characteristic, CONFIG_UUID_LENGTH);
  #endif

  ini.close();

  output_debug_string("Reading config END");
  return true;
}

#endif