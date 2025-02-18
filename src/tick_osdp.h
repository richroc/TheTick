#ifndef TICK_OSDP_H
#define TICK_OSDP_H

// THVD142 should be operating in half-duplex mode (H/F = VCC),
// so driver output pins and receiver input pins are both Y and Z.

#ifdef USE_OSDP
#error "Not implemented"

#else

void osdp_init(void){
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