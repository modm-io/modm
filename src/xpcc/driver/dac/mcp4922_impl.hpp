/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__MCP4922_HPP
	#error	"Don't include this file directly, use 'mcp4922.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Ldac>
void
xpcc::Mcp4922<Spi, Cs, Ldac>::initialize()
{
	//spi.initialize();
	
	Cs::setOutput(xpcc::Gpio::High);
	Ldac::setOutput(xpcc::Gpio::High);
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Ldac>
void
xpcc::Mcp4922<Spi, Cs, Ldac>::setChannelA(uint16_t value, bool doubleGain)
{
	if (doubleGain) {
		writeRegister(BUF | SHDN | (value & 0x0fff));
	}
	else {
		writeRegister(GA | BUF | SHDN | (value & 0x0fff));
	}
}

template <typename Spi, typename Cs, typename Ldac>
void
xpcc::Mcp4922<Spi, Cs, Ldac>::setChannelB(uint16_t value, bool doubleGain)
{
	if (doubleGain) {
		writeRegister(CHANNEL_B | BUF | SHDN | (value & 0x0fff));
	}
	else {
		writeRegister(CHANNEL_B | GA | BUF | SHDN | (value & 0x0fff));
	}
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Ldac>
void
xpcc::Mcp4922<Spi, Cs, Ldac>::update()
{
	xpcc::delayMicroseconds(1); // 40 nanoseconds
	Ldac::reset();
	xpcc::delayMicroseconds(1); // 100 nanoseconds
	Ldac::set();
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Ldac>
void
xpcc::Mcp4922<Spi, Cs, Ldac>::writeRegister(uint16_t value)
{
	Cs::reset();
	
	Spi::write(static_cast<uint8_t>(value >> 8));
	Spi::write(static_cast<uint8_t>(value & 0xff));
	
	Cs::set();
}
