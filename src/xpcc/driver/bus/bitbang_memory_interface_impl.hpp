/*
 * Copyright (c) 2012-2014, Sascha Schade
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__BITBANG_MEMORY_INTERFACE_HPP
#	error "Don't include this file directly, use 'bitbang_memory_interface.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

template <typename PORT, typename CS, typename CD, typename WR>
PORT xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::port;

template <typename PORT, typename CS, typename CD, typename WR>
CS xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::cs;

template <typename PORT, typename CS, typename CD, typename WR>
CD xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::cd;

template <typename PORT, typename CS, typename CD, typename WR>
WR xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::wr;

template <typename PORT, typename CS, typename CD, typename WR>
void xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::initialize()
{
	cs.setOutput(true);
	cd.setOutput(false);
	wr.setOutput(false);
//	port.setOutput(false);
}

template <typename PORT, typename CS, typename CD, typename WR>
void xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::writeRegister(uint8_t reg)
{
	CD::reset();

	CS::reset();

	WR::reset();
	PORT::write(0);

	xpcc::delayMicroseconds(1);
	WR::set();		// Low-to-High strobe
	xpcc::delayMicroseconds(1);


	WR::reset();
	PORT::write(reg);

	xpcc::delayMicroseconds(1);
	WR::set();		// Low-to-High strobe
	xpcc::delayMicroseconds(1);


	CS::set();
}

template <typename PORT, typename CS, typename CD, typename WR>
void xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::writeData(const uint16_t data)
{
	CD::set();

	CS::reset();

	WR::reset();
	PORT::write(data >> 8);

	xpcc::delayMicroseconds(1);
	WR::set();		// Low-to-High strobe
	xpcc::delayMicroseconds(1);


	WR::reset();
	PORT::write(data);

	xpcc::delayMicroseconds(1);
	WR::set();		// Low-to-High strobe
	xpcc::delayMicroseconds(1);

	CS::set();
}

template <typename PORT, typename CS, typename CD, typename WR>
void xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::writeDataMult(const uint16_t data, const uint16_t count)
{
	CD::set();

	CS::reset();

	for (uint_fast16_t ii = count; ii > 0; --ii)
	{
		WR::reset();
		PORT::write(data >> 8);
		xpcc::delayMicroseconds(1);
		WR::set();		// Low-to-High strobe

		WR::reset();
		PORT::write(data);

		xpcc::delayMicroseconds(1);
		WR::set();		// Low-to-High strobe
		xpcc::delayMicroseconds(1);

	}

	CS::set();
}

template <typename PORT, typename CS, typename CD, typename WR>
void xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::writeCommand(const uint8_t command, const uint16_t data)
{
	writeRegister(command);
	writeData(data);
}


template <typename PORT, typename CS, typename CD, typename WR>
void xpcc::BitbangMemoryInterface<PORT, CS, CD, WR>::writeRam(uint8_t * addr, const uint16_t size)
{
	CD::set();

	CS::reset();

	for (int_fast16_t ii = size; ii > 0; --ii)
	{
		WR::reset();
		PORT::write( *(addr++) );

		xpcc::delayMicroseconds(1);
		WR::set();		// Low-to-high strobe
		xpcc::delayMicroseconds(1);
	}

	CS::set();
}
