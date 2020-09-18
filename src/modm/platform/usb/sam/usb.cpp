/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "usb.hpp"

#include <tusb.h>

namespace modm {
namespace platform {

static volatile uint32_t *DBL_TAP_PTR =
    (volatile uint32_t *)(HMCRAMC0_ADDR + HMCRAMC0_SIZE - 4);
constexpr uint32_t DBL_TAP_MAGIC = 0xf01669ef;  // Defined in UF2 bootloader

// Arduino style hack that resets to DFU on disconnect at 1200bps
modm_extern_c void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  (void)itf;  // interface ID, not used
  (void)rts;

  if (!dtr)  // disconnected
  {
    cdc_line_coding_t coding;
    tud_cdc_get_line_coding(&coding);

    if (coding.bit_rate == 1200) {
      *DBL_TAP_PTR = DBL_TAP_MAGIC;
      NVIC_SystemReset();
    }
  }
}

modm_extern_c uint8_t tinyusb_get_serial(uint16_t *serial_str) {
  enum { SERIAL_BYTE_LEN = 16 };

#ifdef __SAMD51__
  uint32_t *id_addresses[4] = {(uint32_t *)0x008061FC, (uint32_t *)0x00806010,
                               (uint32_t *)0x00806014, (uint32_t *)0x00806018};
#else  // samd21
  uint32_t *id_addresses[4] = {(uint32_t *)0x0080A00C, (uint32_t *)0x0080A040,
                               (uint32_t *)0x0080A044, (uint32_t *)0x0080A048};

#endif

  uint8_t raw_id[SERIAL_BYTE_LEN];

  for (int i = 0; i < 4; i++) {
    for (int k = 0; k < 4; k++) {
      raw_id[4 * i + (3 - k)] = (*(id_addresses[i]) >> k * 8) & 0xff;
    }
  }

  const auto fn_nibble = [](uint8_t nibble) {
    return nibble + (nibble > 9 ? 'A' - 10 : '0');
  };

  for (unsigned int i = 0; i < sizeof(raw_id); i++) {
    serial_str[i * 2 + 1] = fn_nibble(raw_id[i] & 0xf);
    serial_str[i * 2] = fn_nibble(raw_id[i] >> 4 & 0xf);
  }

  return sizeof(raw_id) * 2;
}

void USBSerial::initialize() {
  PM->APBBMASK.reg |= PM_APBBMASK_USB;
  PM->AHBMASK.reg |= PM_AHBMASK_USB;
  GenericClockController::connect<ClockPeripheral::Usb>(ClockGenerator::System);
  tusb_init();
}

bool USBSerial::connected() { return tud_ready() && tud_cdc_n_connected(0); }

void USBSerial::loop() { tud_task(); }

bool USBSerial::write(uint8_t c) { return tud_cdc_n_write_char(0, c); }

void USBSerial::flushWriteBuffer() { tud_cdc_n_write_flush(0); }

bool USBSerial::read(uint8_t &c) {
  return tud_cdc_n_read(0, &c, sizeof(uint8_t));
}

MODM_ISR(USB) { tud_int_handler(0); }

}  // namespace platform
}  // namespace modm
