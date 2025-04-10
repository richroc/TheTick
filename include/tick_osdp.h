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

#ifndef TICK_OSDP_H
#define TICK_OSDP_H

#ifdef USE_OSDP

#include <Arduino.h>
#include <osdp.hpp>

void osdp_drain();
void osdp_restore();

int osdp_serial_send_func(void *data, uint8_t *buf, int len);
int osdp_serial_recv_func(void *data, uint8_t *buf, int len);

void osdp_transmit_id(String sendValue, unsigned long bitcount);

int osdp_pd_event_handler(void *data, struct osdp_cmd *cmd);
int osdp_cp_event_handler(void *data, int pd, struct osdp_event *event);

void osdp_disable_transceiver(void);

void osdp_init(void);

void osdp_loop(void);

#else

void osdp_init(void);
void osdp_loop(void);

#endif
#endif