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

// This code originally comes from:
// FSWebServer - Example WebServer with FS backend for esp8266/esp32
// Copyright (c) 2015 Hristo Gochkov. All rights reserved.
// Originally it is a part of the WebServer library for Arduino environment,
// released under GNU Lesser General Public.

#ifndef TICK_HTTP_FILE_HANDLING_H
#define TICK_HTTP_FILE_HANDLING_H

#include "tick_utils.h"

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

#endif