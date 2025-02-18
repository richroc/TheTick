#ifndef TICK_UTILS_H
#define TICK_UTILS_H
#include "tick_default_config.h"

void detachInterrupts(void);
void attachInterrupts(void);
void output_debug_string(String s);

byte hex_to_byte(byte in)
{
  if (in >= '0' && in <= '9')
    return in - '0';
  else if (in >= 'A' && in <= 'F')
    return in - 'A' + 10;
  else if (in >= 'a' && in <= 'f')
    return in - 'a' + 10;
  else
    return in;
}

char c2h(unsigned char c)
{
  // DBG_OUTPUT_PORT.println("c2h: " + String(c));
  return "0123456789abcdef"[0xF & c];
}

uint32_t getChipID() {
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  return chipId;
}

uint32_t readVDCVoltage(void){
  const uint32_t ratio = (1 + (12700/1000));
  return analogReadMilliVolts(PIN_VSENSE) * ratio;
}

void transmit_id(String sendValue, unsigned long bitcount);
void drainD0(void);
void restoreD0(void);
void append_log(String text);
void output_debug_string(String s);
String dhcp_hostname;

#endif