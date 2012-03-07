// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: one_wire.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------
/**
 * \ingroup		connectivity
 * \defgroup	1-wire		1-Wire
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

#ifndef XPCC__ONE_WIRE_HPP
#define XPCC__ONE_WIRE_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/driver/interface.hpp>

namespace xpcc
{
	/**
	 * \ingroup	1-wire
	 */
	namespace one_wire
	{
		/**
		 * \brief	ROM Commands
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
		 * \ingroup	1-wire
		 */
		enum RomCommand
		{
			SEARCH_ROM = 0xf0,
			READ_ROM = 0x33,
			MATCH_ROM = 0x55,
			SKIP_ROM = 0xcc,
			ALARM_SEARCH = 0xec
		};
	}
}

#endif // XPCC__ONE_WIRE_HPP
