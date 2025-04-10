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

#ifdef USE_HTTP

#ifndef USE_WIFI
#error "USE_HTTP must be used with USE_WIFI"
#endif

#include <Arduino.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include "tick_utils.h"
#include "tick_http.h"

WebServer server(80);
File fsUploadFile;

#ifdef USE_OTA_HTTP
#include <HTTPUpdateServer.h>
HTTPUpdateServer httpUpdater;
#endif

void handleTxId() {
  if (!server.hasArg("v")) {
    server.send(500, F("text/plain"), F("BAD ARGS"));
    return;
  }
  String value = server.arg("v");
  // DBG_OUTPUT_PORT.println("handleTxId: " + value);
  String sendValue = value.substring(0, value.indexOf(":"));
  sendValue.toUpperCase();
  unsigned long bitcount = value.substring(value.indexOf(":") + 1).toInt();
  transmit_id(sendValue, bitcount);

  server.send(200, F("text/plain"), "");
}

bool basicAuthFailed() {
  if (strlen(www_username) > 0 && strlen(www_password) > 0) {
    if (!server.authenticate(www_username, www_password)){
      server.requestAuthentication();
      return true;
    }
  }
  return false; // This is good
}


String getContentType(String filename) {
  if (server.hasArg("download"))
    return F("application/octet-stream");
  else if (filename.endsWith(".htm") || filename.endsWith(".html"))
    return F("text/html");
  else if (filename.endsWith(".css"))
    return F("text/css");
  else if (filename.endsWith(".js"))
    return F("application/javascript");
  return "text/plain";
}

bool handleFileRead(String path) {
  DBG_OUTPUT_PORT.println("handleFileRead: " + path);
  if (path.equals(F("/"))) {
    switch (current_tick_mode) {
#ifdef USE_WIEGAND
      case tick_mode_wiegand:
        path = F("/static/wiegand.html");
        break;
#endif
#ifdef USE_CLOCKANDDATA
      case tick_mode_clockanddata:
        path = F("/static/clockanddata.html");
        break;
#endif
      default:
        path = F("/static/dashboard.html");
    }
  }
  if (path.endsWith(F("/"))) path += F("index.html");
  String contentType = getContentType(path);
  String pathWithGz = path + F(".gz");
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz)) path += F(".gz");
    File file = SPIFFS.open(path, "r");
    server.sendHeader("Now", String(millis()));
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() {
  if (basicAuthFailed()) return;
  if (server.uri() != F("/edit")) return;
  HTTPUpload &upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    DBG_OUTPUT_PORT.println("handleFileUpload Name: " + filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile) fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) fsUploadFile.close();
    DBG_OUTPUT_PORT.println("handleFileUpload Size: " + upload.totalSize);
  }
}

void handleFileList() {
  if (basicAuthFailed()) return;
  if (!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  DBG_OUTPUT_PORT.println("handleFileList: " + path);

  File dir = SPIFFS.open(path);

  String output = "[";
  while (File file = dir.openNextFile()) {
    if (strstr(file.path(), ".gz") != NULL) {
      continue;
    }
    if (strstr(file.path(), "/static") != NULL) {
      continue;
    }

    if (output != "[") {
      output += ",";
    }
    output += "{\"type\":\"";
    if (file.isDirectory()) {
      output += "dir";
    } else {
      output += "file";
    }
    output += "\",\"name\":\"";
    output += String(file.path()).substring(1);
    output += "\"}";
  }

  output += "]";

  dir.close();
  server.send(200, "text/json", output);
}

void handleDoS() {
  if (basicAuthFailed())
    return;
  jamming_enable();
  server.send(200, F("text/plain"), "");
  output_debug_string(F("DoS MODE"));
  append_log(F("DoS mode set by API request."));
}

void handleDisableDoS() {
  if (basicAuthFailed())
    return;
  jamming_disable();
  server.send(200, F("text/plain"), "");
  output_debug_string(F("DoS MODE deactivated"));
  append_log(F("DoS mode deactivated by API request."));
}

void handleRestart() {
  if (basicAuthFailed())
    return;
  output_debug_string(F("RESET"));
  append_log(F("Restart requested by user."));
  server.send(200, "text/plain", "OK");
  ESP.restart();
}

void clearConfig(){
  if (basicAuthFailed())
    return;
  clear_config();
  server.send(200, "text/plain", "OK");
  ESP.restart();
}


void http_init(void){

  // SERVER INIT
  server.on("/dos", HTTP_GET, handleDoS);
  server.on("/disabledos", HTTP_GET, handleDisableDoS);
  server.on("/txid", HTTP_GET, handleTxId);
  server.on("/clear", HTTP_GET, clearConfig);

  // list directory
  server.on("/list", HTTP_GET, handleFileList);
  // load editor
  server.on("/edit", HTTP_GET, [](){
    if (basicAuthFailed()) return false;
    if (!handleFileRead("/static/edit.htm")) {
      server.send(404, "text/plain", "FileNotFound");
    }
    return true;
  });

  // first callback is called after the request has ended with all parsed arguments
  // second callback handles file uploads at that location
  server.on(
      "/edit", HTTP_POST, []()
      { server.send(200, "text/plain", ""); },
      handleFileUpload);
  server.on("/restart", HTTP_GET, handleRestart);

  // called when the url is not defined here
  // use it to load content from SPIFFS
  server.onNotFound([](){
    if (basicAuthFailed())
      return false;
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
    return true;
  });

  server.on("/version", HTTP_GET, []() {
    if (basicAuthFailed())
      return false;

    String current_mode = "invalid";
    switch(current_tick_mode){
      #ifdef USE_WIEGAND
      case tick_mode_wiegand:
        current_mode = "wiegand";
        break;
      #endif
      #ifdef USE_CLOCKANDDATA
      case tick_mode_clockanddata:
        current_mode = "clockanddata";
        break;
      #endif
      #ifdef USE_OSDP
      case tick_mode_osdp_cp:
        current_mode = "osdp_cp";
        break;
      case tick_mode_osdp_pd:
        current_mode = "osdp_pd";
        break;
      #endif
    }

    String json = "{\"version\":\"" + String(VERSION) + "\",\"log_name\":\"" + String(log_name) + "\",\"ChipID\":\"" + String(getChipID(), HEX) + "\", \"features\": [" + 

    #ifdef USE_WIEGAND
    "\"wiegand\", " +
    #endif

    #ifdef USE_CLOCKANDDATA
    "\"clockanddata\", " +
    #endif

    #ifdef USE_OSDP
    "\"osdp\", " +
    #endif

    #ifdef USE_BLE
    "\"ble\", " +
    #endif

    #ifdef USE_WIFI
    "\"wifi\", " +
    #endif

    #ifdef USE_MDNS_RESPONDER
    "\"mdns\", " +
    #endif

    #ifdef USE_OTA
    "\"ota\", " +
    #endif
  
    #ifdef USE_OTA_HTTP
    "\"ota_http\", " +
    #endif

    #ifdef USE_SYSLOG
    "\"syslog\", " +
    #endif

    #ifdef USE_LCD
    "\"lcd\", " +
    #endif

    "\"http\"], \"mode\": \"" + current_mode + "\"}\n";
    server.send(200, "text/json", json);
    return true;
  });

  // //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, [](){
    if (basicAuthFailed())
      return false;

    uint32_t data = 
      (digitalRead(wiegand_pin_d0) == HIGH ? 1:0)   * 1 +
      (digitalRead(wiegand_pin_d1) == HIGH ? 1:0)   * 2 +
      (digitalRead(pin_aux) == HIGH ? 1:0) * 4 +
      (digitalRead(pin_reset) == HIGH ? 1:0) * 8;
      
      
    String json = "{";
    json += "\"heap\":" + String(ESP.getFreeHeap());
    json += ", \"vdc\":" + String(readVDCVoltage());
    json += ", \"gpio\":"+String((uint32_t) data);
    json += "}";
    server.send(200, "text/json", json);
    return true;
  });

  server.serveStatic("/static", SPIFFS, "/static", "max-age=86400");
#ifdef USE_OTA_HTTP
  httpUpdater.setup(&server); // This doesn't do authentication
#endif
  server.begin();
}
void http_loop(void){
    // Check for HTTP requests
  server.handleClient();
}

#else

void http_init(void){}
void http_loop(void){}

#endif