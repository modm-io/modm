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

#include <functional>
#include <modm/architecture/interface/interrupt.hpp>
#include <modm/platform/clock/gclk.hpp>
#include <modm/platform/device.hpp>
#include <modm/platform/gpio/base.hpp>

#pragma once

namespace modm {

namespace platform {

MODM_ISR_DECL(USB);

/**
 * USB Serial driver for SAMD devices.
 *
 * @author		Erik Henriksson
 * @ingroup		modm_platform_usb
 */
class USBSerial {
  friend void USB_IRQHandler(void);

 public:
  /**
   * Initializes the USBSerial module.
   */
  static void initialize();

  /**
   * Connects GPIO pins to this USB Serial driver.
   *
   * @tparam Pin
   * 	The GPIO pin to connect this instance to.
   */
  template <class DmPin, class DpPin>
  static void connect() {
    DmPin::template Dm<Peripheral::Usb>::connect();
    DpPin::template Dp<Peripheral::Usb>::connect();
  }

  /**
   * @return True if USB driver is connected to the host.
   */
  static bool connected();

  /**
   * Runs driver loop, must be called in the main loop.
   */
  // FIXME: Make this a fiber.
  static void loop();

  static bool write(uint8_t c);

  static void flushWriteBuffer();

  static bool read(uint8_t& c);
};

}  // namespace platform

}  // namespace modm
