/*
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef ROS_MODM_HARDWARE_H_
#define ROS_MODM_HARDWARE_H_

#include <modm/architecture/interface/uart.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/processing/timer/timestamp.hpp>

/**
 * Interface between rosserial and actual hardware.
 * Must provide:
 *   void init()
 *     To initialize. Called from the NodeHandle::initNode.
 *
 *   int read()
 *     To read a single character from the bytestream.
 *     If no character to read, then return -1.
 *
 *   void write(uint8_t* data, int length)
 *     To write multiple characters to the bytestream.
 *
 *   unsigend long time()
 *     Get time in milliseconds since start. Monotonic increasing clock.
 *
 */
template < class Uart >
class XpccHardware
{
    static_assert(Uart::RxBufferSize >= 512, "UART RX Buffer of supplied UART not large enough. At least 512 bytes of receive buffer are recommended.");
    static_assert(Uart::TxBufferSize >=  64, "UART TX Buffer of supplied UART not large enough. At least 64 bytes of transmit buffer are recommended.");

public:
    XpccHardware() {
    }

    void
    init() {
    }

    int
    read() {
      uint8_t c;
      if (Uart::read(c)) {
        return c;
      } else {
        return -1;
      }
    }

    void
    write(uint8_t* data, int length) {
      Uart::write(data, length);
    }

    unsigned long
    time() {
      modm::Timestamp ts = modm::Clock::now<>();
      return ts.getTime();
    }
};

#endif
