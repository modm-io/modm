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

#pragma once

#include <stdint.h>

namespace modm {

// FIXME: Read these from the NVM controller or get it from modm-devices
constexpr uint32_t kFlashPageSize = 64;
constexpr uint32_t kFlashPages = 2048; // 128kB
constexpr uint32_t kFlashRowSize = 4 * kFlashPageSize;
constexpr uint32_t kFlashSize = kFlashPageSize * kFlashPages;
constexpr uint32_t kProgramOffset = 0x2000;
constexpr uint32_t kStorageSize = (kFlashPageSize * kFlashPages) / 2 - kProgramOffset;

/** On The Air (OTA) update support for SAM devices.
 *
 * NOTE: This uses the second half of flash memory as scratch space for the update. That means
 * only half of the program space is available for applications.
 */
class OTA {
 public:
  /** Initialize the OTA library. */
  static void initialize();
  /** Write len bytes of data to the storage area. Note: len must be multiple of 64 bytes! */
  static void write(uint32_t* data, uint32_t len);
  /** Reset the storage area. */
  static void reset();
  /** Overwrites the program code with the version in the storage area. */
  static void apply();

 private:
  static uint32_t* write_addr_;
};

} // namespace modm
