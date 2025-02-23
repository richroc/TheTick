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

// THVD142 should be operating in half-duplex mode (H/F = VCC),
// so driver output pins and receiver input pins are both Y and Z.

#ifdef USE_OSDP
#error "Not implemented"

#else

void osdp_init(void) {
  // Driver disabled, outputs in High-Z state
  pinMode(PIN_OSDP_DE, OUTPUT);
  pinMode(PIN_OSDP_TX, INPUT);
  digitalWrite(PIN_OSDP_DE, LOW);
  digitalWrite(PIN_OSDP_TX, LOW);
  // Receiver disabled
  pinMode(PIN_OSDP_RE, OUTPUT);
  pinMode(PIN_OSDP_RX, INPUT);
  digitalWrite(PIN_OSDP_RE, HIGH);
  digitalWrite(PIN_OSDP_RX, LOW);

  // Disable terminator
  pinMode(PIN_OSDP_TERM, OUTPUT);
  digitalWrite(PIN_OSDP_TERM, LOW);
}

#endif
#endif