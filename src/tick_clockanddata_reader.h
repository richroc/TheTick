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

#ifndef TICK_CLOCKANDDATA_READER_H
#define TICK_CLOCKANDDATA_READER_H

#include "tick_utils.h"

#ifdef USE_CLOCKANDDATA

static unsigned long clockanddata_last_received = 0;
static String clockanddata_string = "";

void clockanddata_drain_clock(void) {
  detachInterrupts();
  pinMode(clockanddata_pin_clock, OUTPUT);
  digitalWrite(clockanddata_pin_clock, LOW);
}

void clockanddata_restore_clock(void) {
  digitalWrite(clockanddata_pin_clock, HIGH);
  pinMode(clockanddata_pin_clock, INPUT);
  attachInterrupts();
}

void clockanddata_trigger(void) {
  unsigned long now = micros();

  if(now - clockanddata_last_received >= 500){
    if(digitalRead(clockanddata_pin_data) == HIGH){
      clockanddata_string += String("0");
    }else{
      clockanddata_string += String("1");
    }
  }

  clockanddata_last_received=now;
}

// static void wiegand_fix_reader1_string(void)
// {
//   byte loose_bits = reader1_count % 4;
//   if (loose_bits > 0)
//   {
//     byte moving_bits = 4 - loose_bits;
//     byte moving_mask = pow(2, moving_bits) - 1;
//     // DBG_OUTPUT_PORT.println("lb: " + String(loose_bits) + " mb: " + String(moving_bits) + " mm: " + String(moving_mask, HEX));
//     char c = hex_to_byte(reader1_string.charAt(0));
//     for (unsigned long i = 0; i < reader1_string.length(); i++)
//     {
//       reader1_string.setCharAt(i, c2h(c >> moving_bits));
//       c &= moving_mask;
//       c = (c << 4) | hex_to_byte(reader1_string.charAt(i + 1));
//       // DBG_OUTPUT_PORT.println("c: " + String(c, HEX) + " i: " + String(reader1_string.charAt(i)));
//     }
//     reader1_string += String((c >> moving_bits) | reader1_byte, HEX);
//   }
//   reader1_string += ":" + String(reader1_count);
// }

static void clockanddata_reset(void) {
  clockanddata_string.clear();
  clockanddata_last_received = 0;
}


// static void wiegand_transmit_assert(byte bit) {
//   byte pin = bit ? wiegand_pin_d1 : wiegand_pin_d0;
//   digitalWrite(pin, LOW);
//   pinMode(pin, OUTPUT);
//   delayMicroseconds(wiegand_pulse_width);
//   pinMode(pin, INPUT);
//   digitalWrite(pin, HIGH);
//   delayMicroseconds(wiegand_pulse_gap);
// }

void clockanddata_transmit_id(String sendValue, unsigned long bitcount)
{
  detachInterrupts();
  output_debug_string("Sending data: " + sendValue + ":" + String(bitcount));

  const char * bitstream = sendValue.c_str();
  for(unsigned long i = 0; i < bitcount; i++){
    if(bitstream[i] == '0'){
      digitalWrite(clockanddata_pin_data, LOW);
      pinMode(clockanddata_pin_data, OUTPUT);
    }
    delayMicroseconds(clockanddata_pulse_width);

    digitalWrite(clockanddata_pin_clock, LOW);
    pinMode(clockanddata_pin_clock, OUTPUT);
    delayMicroseconds(clockanddata_pulse_width);
    pinMode(clockanddata_pin_clock, INPUT);
    digitalWrite(clockanddata_pin_clock, HIGH);
    delayMicroseconds(clockanddata_pulse_width);

    pinMode(clockanddata_pin_data, INPUT);
    digitalWrite(clockanddata_pin_data, HIGH);
  }

  attachInterrupts();
}

void clockanddata_loop(void){
  if(clockanddata_string.length() > 0 && (micros() - clockanddata_last_received >= 5000)) {
    // noInterrupts();
    String received_card = clockanddata_string + String(":") + String(clockanddata_string.length());
    clockanddata_reset();
    // interrupts();
    card_read_handler(received_card);
  }
}

#else

// #error "Clockanddata support is currently required"

#endif
#endif