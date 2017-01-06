/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * @ingroup		interface
 * @defgroup	1-wire		1-Wire
 *
 * 1-Wire is a device communications bus system designed by
 * Dallas Semiconductor Corp. that provides low-speed data, signaling and
 * power over a single signal.[1] 1-Wire is similar in concept to I²C, but
 * with lower data rates and longer range. It is typically used to communicate
 * with small inexpensive devices such as digital thermometers and
 * weather instruments.
 *
 * One distinctive feature of the bus is the possibility to use only two
 * wires: data and ground. To accomplish this, 1-wire devices include an
 * 800 pF capacitor to store charge, and power the device during periods where
 * the data line is used for data.
 */

#ifndef MODM_ONE_WIRE_HPP
#define MODM_ONE_WIRE_HPP

#include <modm/architecture/utils.hpp>
#include <modm/architecture/interface.hpp>

namespace modm
{

/// @ingroup	1-wire
namespace one_wire
{

/**
 * ROM Commands
 *
 * After the bus master has detected a presence pulse, it can issue a
 * ROM command. These commands operate on the unique 64-bit ROM codes
 * of each slave device and allow the master to single out a specific
 * device if many are present on the 1-Wire bus.
 *
 * These commands also allow the master to determine how many and what
 * types of devices are present on the bus or if any device has
 * experienced an alarm condition.
 *
 * There are five ROM commands, and each command is 8 bits long.
 *
 * @ingroup	1-wire
 */
enum RomCommand
{
	SEARCH_ROM = 0xf0,
	READ_ROM = 0x33,
	MATCH_ROM = 0x55,
	SKIP_ROM = 0xcc,
	ALARM_SEARCH = 0xec
};

}	// namespace one_wire

}	// namespace modm

#endif // MODM_ONE_WIRE_HPP
