// coding: utf-8
/*
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_DS1302_HPP
#error "Do not include ds1302_impl.hpp. Only include ds1302.hpp"
#endif

template < class PinSet >
void
modm::Ds1302< PinSet >::initialize()
{
	Ce::reset();
	Sclk::reset();
	Io::setInput();
}

template< class PinSet >
void
modm::Ds1302< PinSet >::writeByte(uint8_t byte)
{
	modm::this_fiber::sleep_for(DELAY_CE);
	Io::setOutput();

	for (uint8_t ii = 8; ii > 0; --ii)
	{
		modm::this_fiber::sleep_for(DELAY_CLK);
		Sclk::reset();
		Io::set(byte & 0x01);
		modm::this_fiber::sleep_for(DELAY_CLK);
		Sclk::set();

		byte >>= 1;
	}
	modm::this_fiber::sleep_for(DELAY_CLK);
}

template< class PinSet >
void
modm::Ds1302< PinSet >::write(const uint8_t addr, const uint8_t data)
{
	Ce::set();
	writeByte(addr);
	writeByte(data);

	// Cleanup
	Sclk::reset();
	modm::this_fiber::sleep_for(DELAY_CLK);
	Io::setInput();
	Ce::reset();
	modm::this_fiber::sleep_for(DELAY_CE);
}

template< class PinSet >
uint8_t
modm::Ds1302< PinSet >::read(const uint8_t addr)
{
	Ce::set();
	writeByte(addr);

	uint8_t ret = 0;

	Io::setInput();
	modm::this_fiber::sleep_for(DELAY_CLK);
	for (uint8_t ii = 8; ii > 0; --ii)
	{
		bool rr = Io::read();
		ret >>= 1;
		ret |= (rr << 7);
		Sclk::set();
		modm::this_fiber::sleep_for(DELAY_CLK);
		Sclk::reset();
		modm::this_fiber::sleep_for(DELAY_CLK);
	}

	Ce::reset();

	return ret;
}


template< class PinSet >
void
modm::Ds1302< PinSet >::readRtc(ds1302::Data &storage)
{
	Ce::set();
	writeByte(0xbf); // Burst Read
	Io::setInput();

	// Falling edge of SCLK will trigger DS1302 output
	Sclk::reset();

	// Wait for stable output
	modm::this_fiber::sleep_for(DELAY_CE);

	for (uint8_t jj = 0; jj < MODM_ARRAY_SIZE(storage.data); ++jj)
	{
		uint8_t ret = 0;
		for (uint8_t ii = 8; ii > 0; --ii)
		{
			bool rr = Io::read();
			ret >>= 1;
			ret |= (rr << 7);
			Sclk::set();
			modm::this_fiber::sleep_for(DELAY_CLK);

			// Falling edge of SCLK will trigger DS1302 output
			Sclk::reset();
			modm::this_fiber::sleep_for(DELAY_CLK);
		}
		storage.data[jj] = ret;
	}

	Ce::reset();
}
