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

#include "ota.hpp"
#include "../device.hpp"

namespace modm {
namespace {

modm_always_inline
static void flashWaitReady() {
  while (!NVMCTRL->INTFLAG.bit.READY);
}

modm_always_inline
static void flashErase(uint32_t* addr, uint32_t len) {
  for (uint32_t row = 0; row < (len + kFlashRowSize - 1) / kFlashRowSize; ++row) {
    NVMCTRL->ADDR.reg = (uint32_t) addr / 2;
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
    flashWaitReady();
    addr += kFlashRowSize / 4;
  }
}

modm_always_inline
static int flashRowEquals(uint32_t* dest, uint32_t* src) {
  for (uint32_t i = 0; i < kFlashPageSize; i += 4) {
    if (dest++ != src++) return false;
  }
  return true;
}

modm_always_inline
static int flashCopy(uint32_t* dest, uint32_t* src, uint32_t len) {
  uint32_t bytes_left = len;
  while (bytes_left) {
    if (flashRowEquals(dest, src)) {
      dest += kFlashPageSize;
      src += kFlashPageSize;
      continue;
    }
    for (uint32_t i = 0; i < kFlashPageSize && bytes_left; i += 4) {
      *(dest++) = *src++;
      bytes_left -= 4;
    }
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
    flashWaitReady();
  }
  return len;
}

// Needs to be stored in RAM as this method overwrites the program code on flash.
__attribute__ ((long_call, noinline, section(".ramcode")))
static void copyFlashAndReset(uint32_t* dest, uint32_t* src, uint32_t len) {
  __disable_irq();
  flashErase(dest, kStorageSize);
  flashCopy(dest, src, len);
  NVIC_SystemReset();
}

} // namespace

void OTA::initialize() {
  NVMCTRL->CTRLB.bit.MANW = 1; // Disable auto page writes
  reset();
}

void OTA::write(uint32_t* data, uint32_t len) {
  write_addr_ += flashCopy(write_addr_, data, len);
}

void OTA::reset() {
  write_addr_= (uint32_t*) (kProgramOffset + kFlashSize / 2);
  flashErase(write_addr_, kStorageSize);
}

void OTA::apply() {
  uint32_t* src = (uint32_t*) (kProgramOffset + kFlashSize / 2);
  uint32_t len = write_addr_ - src;
  copyFlashAndReset((uint32_t*) kProgramOffset, src, len);
}

uint32_t* OTA::write_addr_;

} // namespace modm
