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

#include <esp32-hal-gpio.h>
#include <esp32-hal-rgb-led.h>
#include "tick_heartbeat.h"

#ifdef RGB_BUILTIN

struct Color {
  int r, g, b;
};

void heartbeat_init(void) {}

void heartbeat_loop(void) {
  constexpr Color colors[] = {
      {32, 49, 84}, {227, 65, 70}, {124, 138, 165}, {69, 94, 124}};

  static int currentColorIndex = 0;
  static unsigned long startTime = millis();
  static unsigned long elapsed = millis();

  unsigned long now = millis();
  if (elapsed - now > 10) {
    elapsed = now - startTime;

    float t = float(elapsed) / 1000;

    if (t >= 1.0) {
      startTime = now;
      currentColorIndex = (currentColorIndex + 1) % 4;
      t = 0.0;
    }

    int nextColorIndex = (currentColorIndex + 1) % 4;

    Color c1 = colors[currentColorIndex];
    Color c2 = colors[nextColorIndex];

    int r = c1.r + (c2.r - c1.r) * t;
    int g = c1.g + (c2.g - c1.g) * t;
    int b = c1.b + (c2.b - c1.b) * t;

    neopixelWrite(RGB_BUILTIN, r/5, g/5, b/5);

    elapsed = now;
  }
}

#else

void heartbeat_init(void) { pinMode(LED_BUILTIN, OUTPUT); }

void heartbeat_loop(void) {
  static unsigned int counter = 0;
  static unsigned long step = 0;
  unsigned long now = micros();

  if(now - step > 500000){
    step = now;
    if (counter++ % 2) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}

#endif