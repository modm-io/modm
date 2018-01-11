#ifndef ROS_XPCC_HARDWARE_H_
#define ROS_XPCC_HARDWARE_H_

#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/driver/clock.hpp>
#include <xpcc/processing/timer/timestamp.hpp>

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
      xpcc::Timestamp ts = xpcc::Clock::now<>();
      return ts.getTime();
    }
};

#endif
