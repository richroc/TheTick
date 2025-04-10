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
#define WIEGAND_PULSE_WIDTH 34       // length of asserted pulse in microSeconds
#define WIEGAND_PULSE_GAP \
  2000 - WIEGAND_PULSE_WIDTH          // delay between pulses in microSeconds
#define CLOCKANDDATA_PULSE_WIDTH 300  // length of half of clock signal

#define TICK_MODE 0

// Features
// #define USE_WIFI
// #define USE_HTTP
// #define USE_SYSLOG
// #define USE_OTA
// #define USE_OTA_HTTP
// #define USE_LCD
// #define USE_MDNS_RESPONDER
// #define USE_OSDP
// #define USE_BLE
// #define USE_WIEGAND

// Pin number assignments
#define PIN_D0 0
#define PIN_D1 1
#define PIN_AUX 3
#define PIN_VSENSE 4
#define CONF_RESET 9

#define PIN_OSDP_TERM 5
#define PIN_OSDP_DE 7
#define PIN_OSDP_RE 10
#define PIN_OSDP_RX SOC_RX0
#define PIN_OSDP_TX SOC_TX0

#define LOG_NAME "TheTick"
#define MDNSHOST "TheTick"

#define AP_ENABLE true
#define AP_HIDDEN false
#define AP_SSID "TheTick-config"
#define AP_PSK "accessgranted"
#define AP_IP "192.168.4.1"

#define STATION_SSID ""
#define STATION_PSK ""

#define SYSLOG_HOSTNAME "TheTick"
#define SYSLOG_SERVICE "accesscontrol"
#define SYSLOG_PORT 514
#define SYSLOG_SERVER "0.0.0.0"
#define SYSLOG_PRIORITY 36

#define OTA_PASSWORD "ExtraSpecialPassKey"

#define WWW_USERNAME ""
#define WWW_PASSWORD ""

#define DOS_CARD_ID "7fffffff:31"

#define BLE_UUID_WIEGAND_SERVICE "f498124f-2137-4615-9859-30eb4cecffb5"
#define BLE_UUID_WIEGAND_CHARACTERISTIC "beb5483e-36e1-4688-b7f5-ea07361baaaa"
#define BLE_PASSKEY 123456

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels
#define SCREEN_ADDRESS \
  0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// ---------------------------------------------------------------

// // Default settings used when no configuration file exists
#define CONFIG_VAR_LENGTH 20
#define CONFIG_PASSWORD_LENGTH 64
#define CONFIG_UUID_LENGTH 37
#define CONFIG_SSID_LENGTH 33

extern char log_name[CONFIG_VAR_LENGTH];

enum tick_mode {
  tick_mode_disabled,
  tick_mode_wiegand,
  tick_mode_clockanddata,
  tick_mode_osdp_pd,
  tick_mode_osdp_cp
};

extern enum tick_mode current_tick_mode;

#ifdef USE_WIEGAND
extern int wiegand_pin_d0;
extern int wiegand_pin_d1;
extern int wiegand_pulse_width;
extern int wiegand_pulse_gap;
#endif

#ifdef USE_CLOCKANDDATA
extern int clockanddata_pin_clock;
extern int clockanddata_pin_data;
extern int clockanddata_pulse_width;
#endif

#ifdef USE_OSDP
extern int osdp_pin_term;
extern int osdp_pin_de;
extern int osdp_pin_re;
extern int osdp_pin_rx;
extern int osdp_pin_tx;
extern bool osdp_terminator;
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

void set_default_config(void);
bool loadConfig();

#endif
