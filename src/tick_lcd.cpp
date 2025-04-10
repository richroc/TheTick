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

#include "tick_lcd.h"

#ifdef USE_LCD
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <tick_default_config.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void display_string(String data) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(data);
  display.display();
}

void display_receivedData(const uint8_t* data, const uint8_t bits,
                          const char* message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(message);
  display.print(bits);
  display.println("bits");

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x"));

  uint8_t bytes = (bits + 7) / 8;
  for (int i = 0; i < bytes; i++) {
    display.print(data[i] >> 4, 16);
    display.print(data[i] & 0xF, 16);
  }
  display.display();
}

void display_init() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display_string(F("MY BODY IS READY"));
}
#else

void display_string(String data) {}
void display_receivedData(uint8_t* data, uint8_t bits, const char* message) {}
void display_init() {}

#endif