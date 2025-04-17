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

#include <Arduino.h>
#include "tick_utils.h"

void osdp_disable_transceiver(void) {
  // Driver disabled, outputs in High-Z state
  pinMode(osdp_pin_de, OUTPUT);
  pinMode(osdp_pin_tx, INPUT);
  digitalWrite(osdp_pin_de, LOW);
  digitalWrite(osdp_pin_tx, LOW);
  // Receiver disabled
  pinMode(osdp_pin_re, OUTPUT);
  pinMode(osdp_pin_rx, INPUT);
  digitalWrite(osdp_pin_re, HIGH);
  digitalWrite(osdp_pin_rx, LOW);

  // Disable terminator
  if(osdp_pin_term != -1) {
    pinMode(osdp_pin_term, OUTPUT);
    digitalWrite(osdp_pin_term, LOW);
  }

}

#ifdef USE_OSDP

#include <osdp.hpp>
#include "tick_osdp.h"

OSDP::PeripheralDevice pd;
OSDP::ControlPanel cp;

uint8_t osdp_scbk_raw[16];
uint8_t osdp_mk_raw[16];
osdp_pd_info_t pd_info[] = {{
    .name = "pd[101]",
    .baud_rate = -1,
    .address = -1,
    .flags = 0,
    .id =
        {
            .version = 1,
            .model = 153,
            .vendor_code = 31337,
            .serial_number = 0x01020304,
            .firmware_version = 0x0A0B0C0D,
        },
    .cap =
        (struct osdp_pd_cap[]){
            {.function_code = OSDP_PD_CAP_READER_LED_CONTROL,
             .compliance_level = 1,
             .num_items = 1},
            {.function_code = OSDP_PD_CAP_READERS,
             .compliance_level = 1,
             .num_items = 1},
            {.function_code = OSDP_PD_CAP_CARD_DATA_FORMAT,
             .compliance_level = 1,
             .num_items = 1},
            {static_cast<uint8_t>(-1), 0, 0} /* Sentinel */
        },
    .channel = {.data = nullptr,
                .id = 0,
                .recv = nullptr,
                .send = nullptr,
                .flush = nullptr},
    .scbk = nullptr,
}};

void osdp_drain(){
  digitalWrite(wiegand_pin_d0, LOW);
  digitalWrite(wiegand_pin_d1, LOW);
}

void osdp_restore(){
  digitalWrite(wiegand_pin_d0, HIGH);
  digitalWrite(wiegand_pin_d1, HIGH);
}

int osdp_serial_send_func(void *data, uint8_t *buf, int len) {
  (void)(data);

  return osdp_serial->write(buf, (size_t)len);
}

int osdp_serial_recv_func(void *data, uint8_t *buf, int len) {
  (void)(data);

  return osdp_serial->read(buf, (size_t)len);
}

void osdp_transmit_id(String sendValue, unsigned long bitcount) {
  osdp_event card_event = {
      .type = OSDP_EVENT_CARDREAD,
      .flags = 0,
      .cardread = osdp_event_cardread{
          .reader_no = 0, .format = OSDP_CARD_FMT_RAW_WIEGAND, .direction = 1}};

  card_event.cardread.length = bitcount;
  for (int i = 0; i < sendValue.length() / 2; i++) {
    card_event.cardread.data[i] =
        hex_to_byte(sendValue[i * 2]) << 4 | hex_to_byte(sendValue[i * 2 + 1]);
  }

  pd.submit_event(&card_event);
}

int osdp_pd_event_handler(void *data, struct osdp_cmd *cmd) {
  (void)(data);

  output_debug_string("Received an OSDP command!");
  return 0;
}

int osdp_cp_event_handler(void *data, int pd, struct osdp_event *event) {
  (void)(data);

  output_debug_string("Received an OSDP event!");
  return 0;
}

void osdp_init(void) {
  if (current_tick_mode == tick_mode_osdp_pd ||
      current_tick_mode == tick_mode_osdp_cp) {
    // disable Wiegand interface
    pinMode(wiegand_pin_d0, OUTPUT);
    pinMode(wiegand_pin_d1, OUTPUT);
    digitalWrite(wiegand_pin_d0, HIGH);
    digitalWrite(wiegand_pin_d1, HIGH);

    // configure RS485 transceiver
    pinMode(osdp_pin_de, OUTPUT);
    digitalWrite(osdp_pin_de, LOW);

    pinMode(osdp_pin_re, OUTPUT);
    digitalWrite(osdp_pin_re, LOW);

    pinMode(osdp_pin_tx, OUTPUT);
    digitalWrite(osdp_pin_tx, LOW);

    pinMode(osdp_pin_rx, INPUT);
    digitalWrite(osdp_pin_rx, LOW);

    if(osdp_pin_term != -1) {
      pinMode(osdp_pin_term, OUTPUT);
      digitalWrite(osdp_pin_term, osdp_terminator ? HIGH : LOW);
    }

    // This configuration at the first glance is a bit counterintuitive:
    // - receiver is permanently enabled, as:
    //    - it is used for collision detection
    //    - echo is suppresed by UART hardware
    // - hardware flow in the UART controller is DISABLED, as it doesn't support
    // half-duplex communication
    // - RTS signal is controlled the software UART driver
    // - CTS signal is not used
    // https://docs.espressif.com/projects/esp-idf/en/v5.0/esp32c3/api-reference/peripherals/uart.html#overview-of-rs485-specific-communication-options

    osdp_serial->begin(osdp_baudrate, SERIAL_8N1, osdp_pin_rx, osdp_pin_tx);
    osdp_serial->setMode(UART_MODE_RS485_HALF_DUPLEX);
    osdp_serial->setPins(osdp_pin_rx, osdp_pin_tx, -1, osdp_pin_de);
    osdp_serial->setHwFlowCtrlMode(UART_HW_FLOWCTRL_DISABLE, 122);

    pd_info[0].channel.recv = osdp_serial_recv_func;
    pd_info[0].channel.send = osdp_serial_send_func;
    pd_info[0].baud_rate = osdp_baudrate;
    pd_info[0].address = osdp_address;

    if (strlen(osdp_scbk) == 32) {
      for (int i = 0; i < 16; i++) {
        osdp_scbk_raw[i] = hex_to_byte(osdp_scbk[i * 2]) << 4 |
                           hex_to_byte(osdp_scbk[i * 2 + 1]);
      }
      pd_info[0].scbk = osdp_scbk_raw;
    } else {
      pd_info[0].scbk = nullptr;
    }

    if(current_tick_mode == tick_mode_osdp_cp) {
      pd_info[0].cap = nullptr;
      pd_info[0].id = {};
    }

    switch (current_tick_mode) {
      case tick_mode_osdp_pd:
        pd.logger_init("osdp::pd", OSDP_LOG_DEBUG, NULL);
        pd.setup(pd_info);
        pd.set_command_callback(osdp_pd_event_handler, nullptr);
        output_debug_string("OSDP Peripheral Device started");
        break;
      case tick_mode_osdp_cp:
        cp.logger_init("osdp::cp", OSDP_LOG_DEBUG, NULL);
        cp.setup(1, pd_info);
        cp.set_event_callback(osdp_cp_event_handler, nullptr);
        output_debug_string("OSDP Control Panel started");
        break;
    }

  } else {
    osdp_disable_transceiver();
  }
}

void osdp_loop(void) {
  static unsigned long elapsed = millis();
  unsigned long now = millis();
  if (elapsed - now > 50) {
    if (current_tick_mode == tick_mode_osdp_pd) {
      pd.refresh();
    } else if (current_tick_mode == tick_mode_osdp_cp) {
      cp.refresh();
      cp.submit_command(0, nullptr);
    }
  }
}

#else

void osdp_init(void) { osdp_disable_transceiver(); }
void osdp_loop(void) {}

#endif
