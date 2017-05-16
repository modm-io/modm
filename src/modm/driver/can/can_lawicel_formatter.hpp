/*
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2015, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_CAN_LAWICEL_FORMATTER_HPP
#define MODM_CAN_LAWICEL_FORMATTER_HPP

#include <modm/architecture/interface/can_message.hpp>

namespace modm
{

/**
 * Converts MODM CAN messages (modm::can::Message) to and from the Lawicel string format (char *).
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
	hexToByte(const char *s, bool& error)
	{
		return (charToByte(s[0], error) << 4) | charToByte(s[1], error);
	}

	static uint8_t
	charToByte(const char cc, bool& error);

	static char
	byteToHex(uint8_t num);
};

}	// namespace modm

#endif // MODM_CAN_LAWICEL_FORMATTER_HPP
