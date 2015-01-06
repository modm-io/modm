// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_CAN_LAWICEL_FORMATTER_HPP
#define XPCC_CAN_LAWICEL_FORMATTER_HPP

#include <xpcc/architecture/interface/can_message.hpp>

namespace xpcc
{

/**
 * Converts XPCC CAN messages (xpcc::can::Message) to and from the Lawicel string format (char *).
 *
 * Lawicel AB offers medium sized CAN to USB
 * and CAN to RS232 converters. Their data format is widely used, including
 * RCA's BotControl Java GUI and Fabian's USB2CAN dongle.
 *
 * This converter only understands messages of type 'r', 't', 'R' and 'T' which
 * transmits CAN frames. It does not understand commands to change the baud rate et cetera.
 *
 * @ingroup driver_can
 * @see http://www.lawicel.com/
 */
class CanLawicelFormatter
{
public:
	static bool
	convertToCanMessage(const char* in, can::Message& out);

	static bool
	convertToString(const can::Message& in, char* out);

private:
	static inline uint8_t
	hexToByte(const char *s)
	{
		return (charToByte(s) << 4) | charToByte(s + 1);
	}

	static uint8_t
	charToByte(const char *s);

	static char
	byteToHex(uint8_t num);
};

}	// namespace xpcc

#endif // XPCC_CAN_LAWICEL_FORMATTER_HPP
