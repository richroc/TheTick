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


#include <SPIFFS.h>
#include <nvs_flash.h>
#include "tick_utils.h"

#ifndef TICK_RESET_H
#define TICK_RESET_H

volatile unsigned int reset_button_counter = 0;

void clear_config(void){
    File f;
    f = SPIFFS.open(CONFIG_FILE, "w");
    f.close();

    f = SPIFFS.open(LOG_FILE, "w");
    f.close();

    SPIFFS.end();

    nvs_flash_erase();
    nvs_flash_init();
}

void reset_loop(void){
  if (reset_button_counter > 3) {
    output_debug_string(F("CONFIG RESET"));

    clear_config();

    delay(5000);

    ESP.restart();
  }
}

#endif