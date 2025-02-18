#ifndef TICK_WIEGAND_READER_H
#define TICK_WIEGAND_READER_H

#include "tick_utils.h"

void reader1_append(int value)
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

void reader1_wiegand_trigger(void)
{
  if(digitalRead(PIN_D0) == LOW)
    reader1_append(0);
  
  if(digitalRead(PIN_D1) == LOW)
    reader1_append(1);
}

void fix_reader1_string(void)
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

void reader1_reset()
{
  reader1_byte = 0;
  reader1_count = 0;
  reader1_string = "";
}


void transmit_assert(byte bit) {
  byte pin = bit ? PIN_D1 : PIN_D0;
  digitalWrite(pin, LOW);
  pinMode(pin, OUTPUT);
  delayMicroseconds(PULSE_WIDTH);
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
  delayMicroseconds(PULSE_GAP);
}

void transmit_id(String sendValue, unsigned long bitcount)
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
      transmit_assert(0);
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
      transmit_assert(b);
    }
    excess_bits = 0;
  }
  // DBG_OUTPUT_PORT.println();
  attachInterrupts();
}

#endif