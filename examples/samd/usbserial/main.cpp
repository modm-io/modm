/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <tusb.h>

#include <modm/board.hpp>

using namespace Board;
using namespace std::chrono_literals;

int main() {
  Board::initialize();

  // Output 500hz PWM on D12 so we can validate the GCLK0 clock speed.
  PM->APBCMASK.reg |= PM_APBCMASK_TCC0;
  TCC0->PER.bit.PER = SystemClock::Frequency / 1000;
  TCC0->CC[3].bit.CC = SystemClock::Frequency / 2000;
  TCC0->CTRLA.bit.ENABLE = true;
  D12::Wo3<Peripheral::Tcc0>::connect();
  GenericClockController::connect<ClockPeripheral::Tcc0>(
      ClockGenerator::System);

  Led::set();
  USBSerial::initialize();
  USBSerial::connect<GpioA24, GpioA25>();
  using USBIODevice =
      modm::IODeviceWrapper<USBSerial, modm::IOBuffer::BlockIfFull>;
  USBIODevice usb_io_device;
  modm::IOStream usb_stream(usb_io_device);

  while (!USBSerial::connected()) USBSerial::loop();

  usb_stream << "Hello world!" << modm::endl << modm::flush;

  Led::reset();

  while (1) USBSerial::loop();
  return 0;
}
