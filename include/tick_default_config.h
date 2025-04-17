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

#ifndef TICK_DEFAULT_CONFIG_H
#define TICK_DEFAULT_CONFIG_H

#include <IPAddress.h>
#include <HardwareSerial.h>

#include <cstddef>

#define VERSION "2317"

#define HOSTNAME "TheTick-" // Hostname prefix for DHCP/OTA
#define CONFIG_FILE "/config.txt"
#define LOG_FILE "/log.txt"
#define DBG_OUTPUT_PORT Serial       // This could be a file with some hacking
#define CARD_LEN 4                   // minimum card length in bits


#define LOG_NAME "TheTick"
#define MDNSHOST "TheTick"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
///< See datasheet for address; 0x3D for 128x64, 0x3C for 128x32
#define SCREEN_ADDRESS  0x3C  

// ---------------------------------------------------------------

// // Default settings used when no configuration file exists
#define CONFIG_VAR_LENGTH 20
#define CONFIG_PASSWORD_LENGTH 64
#define CONFIG_UUID_LENGTH 37
#define CONFIG_SSID_LENGTH 33


extern int pin_aux, pin_vsense, pin_reset;

extern char log_name[CONFIG_VAR_LENGTH];

enum tick_mode {
  tick_mode_disabled,
  tick_mode_wiegand,
  tick_mode_clockanddata,
  tick_mode_osdp_pd,
  tick_mode_osdp_cp
};

extern enum tick_mode current_tick_mode;

extern int wiegand_pin_d0;
extern int wiegand_pin_d1;
#ifdef USE_WIEGAND
extern int wiegand_pulse_width;
extern int wiegand_pulse_gap;
#endif

#ifdef USE_CLOCKANDDATA
extern int clockanddata_pin_clock;
extern int clockanddata_pin_data;
extern int clockanddata_pulse_width;
#endif

extern int osdp_pin_term;
extern int osdp_pin_de;
extern int osdp_pin_re;
extern int osdp_pin_rx;
extern int osdp_pin_tx;
extern bool osdp_terminator;
#ifdef USE_OSDP
extern HardwareSerial *osdp_serial;

#define OSDP_BAUDRATE 115200

extern int osdp_baudrate;
extern int osdp_address;

extern char osdp_scbk[CONFIG_PASSWORD_LENGTH];
extern char osdp_mk[CONFIG_PASSWORD_LENGTH];
#endif

#ifdef USE_BLE
extern char ble_uuid_wiegand_service[CONFIG_UUID_LENGTH];
extern char ble_uuid_wiegand_characteristic[CONFIG_UUID_LENGTH];
extern uint32_t ble_passkey;
#endif

#ifdef USE_WIFI
extern bool ap_enable;
extern bool ap_hidden;
extern char ap_ssid[CONFIG_SSID_LENGTH];
extern IPAddress ap_ip;
extern char ap_psk[CONFIG_PASSWORD_LENGTH];
extern char station_ssid[CONFIG_SSID_LENGTH];
extern char station_psk[CONFIG_PASSWORD_LENGTH];
#endif

#ifdef USE_MDNS_RESPONDER
extern char mDNShost[CONFIG_VAR_LENGTH];
#endif

extern char DoS_id[CONFIG_PASSWORD_LENGTH];

#ifdef USE_OTA
extern char ota_password[CONFIG_PASSWORD_LENGTH];
#endif

#ifdef USE_HTTP
extern char www_username[CONFIG_VAR_LENGTH];
extern char www_password[CONFIG_PASSWORD_LENGTH];
#endif

#ifdef USE_SYSLOG
extern IPAddress syslog_server;
extern unsigned int syslog_port;
extern char syslog_service_name[CONFIG_VAR_LENGTH];
extern char syslog_host[CONFIG_VAR_LENGTH];
extern byte syslog_priority;
#endif

bool loadConfig(const char *filename);

#endif
