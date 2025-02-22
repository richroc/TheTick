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

// This code is based on ESPKey, by Kenny McElroy
// https://github.com/octosavvi/ESPKey/blob/master/ESPKey.ino
// released under GNU General Public License version 2.

#ifndef TICK_WIEGAND_READER_H
#define TICK_WIEGAND_READER_H

#include "tick_utils.h"

#ifdef USE_WIEGAND

byte reader1_byte = 0;
String reader1_string = "";
int reader1_count = 0;
unsigned long reader1_millis = 0;

void wiegand_drainD0(void) {
  detachInterrupts();
  pinMode(wiegand_pin_d0, OUTPUT);
  digitalWrite(wiegand_pin_d0, LOW);
}

void wiegand_restoreD0(void) {
  digitalWrite(wiegand_pin_d0, HIGH);
  pinMode(wiegand_pin_d0, INPUT);
  attachInterrupts();
}

static void IRAM_ATTR wiegand_reader_append(int value)
{
  reader1_count++;
  reader1_millis = millis();
  reader1_byte = reader1_byte << 1;
  reader1_byte |= 1 & value;
  if (reader1_count % 4 == 0)
  {
    reader1_string += String(reader1_byte, HEX);
    reader1_byte = 0;
  }
}

void IRAM_ATTR reader1_wiegand_trigger(void)
{
  if(digitalRead(wiegand_pin_d0) == LOW)
    wiegand_reader_append(0);
  
  if(digitalRead(wiegand_pin_d1) == LOW)
    wiegand_reader_append(1);
}

static void wiegand_fix_reader1_string(void)
{
  byte loose_bits = reader1_count % 4;
  if (loose_bits > 0)
  {
    byte moving_bits = 4 - loose_bits;
    byte moving_mask = pow(2, moving_bits) - 1;
    // DBG_OUTPUT_PORT.println("lb: " + String(loose_bits) + " mb: " + String(moving_bits) + " mm: " + String(moving_mask, HEX));
    char c = hex_to_byte(reader1_string.charAt(0));
    for (unsigned long i = 0; i < reader1_string.length(); i++)
    {
      reader1_string.setCharAt(i, c2h(c >> moving_bits));
      c &= moving_mask;
      c = (c << 4) | hex_to_byte(reader1_string.charAt(i + 1));
      // DBG_OUTPUT_PORT.println("c: " + String(c, HEX) + " i: " + String(reader1_string.charAt(i)));
    }
    reader1_string += String((c >> moving_bits) | reader1_byte, HEX);
  }
  reader1_string += ":" + String(reader1_count);
}

static void wiegand_reader1_reset()
{
  reader1_byte = 0;
  reader1_count = 0;
  reader1_string = "";
}

static void wiegand_transmit_assert(byte bit) {
  byte pin = bit ? wiegand_pin_d1 : wiegand_pin_d0;
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);
  delayMicroseconds(wiegand_pulse_width);
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
  delayMicroseconds(wiegand_pulse_gap);
}

void wiegand_transmit_id(String sendValue, unsigned long bitcount)
{
  detachInterrupts();
  output_debug_string("Sending data: " + sendValue + ":" + String(bitcount));
  unsigned long bits_available = sendValue.length() * 4;
  unsigned long excess_bits = 0;
  if (bits_available > bitcount)
  {
    excess_bits = bits_available - bitcount;
    sendValue = sendValue.substring(excess_bits / 4);
    excess_bits %= 4;
    // DBG_OUTPUT_PORT.print("sending: " + sendValue + " with excess bits: " + excess_bits + "\n\t");
  }
  else if (bits_available < bitcount)
  {
    for (unsigned long i = bitcount - bits_available; i > 0; i--)
    {
      wiegand_transmit_assert(0);
    }
  }
  for (unsigned long i = 0; i < sendValue.length(); i++)
  {
    char c = hex_to_byte(sendValue.charAt(i));
    // DBG_OUTPUT_PORT.println("i:" + String(i) + " c:0x" + String(c,HEX));
    for (short x = 3 - excess_bits; x >= 0; x--)
    {
      byte b = bitRead(c, x);
      // DBG_OUTPUT_PORT.println("x:" + String(x) + " b:" + b);
      wiegand_transmit_assert(b);
    }
    excess_bits = 0;
  }
  // DBG_OUTPUT_PORT.println();
  attachInterrupts();
}

void wiegand_loop(void){
  if (reader1_count >= CARD_LEN && (reader1_millis + 5 <= millis() || millis() < 10)) {
    wiegand_fix_reader1_string();
    card_read_handler(reader1_string);
    wiegand_reader1_reset();
  }
}

#else

#error "Wiegand support is currently required"

#endif
#endif