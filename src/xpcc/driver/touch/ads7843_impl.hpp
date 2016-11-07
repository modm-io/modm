/*
 * Copyright (c) 2009-2010, 2012-2013, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, Daniel Krebs
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ADS7843_HPP
#	error "Don't include this file directly, use 'ads7843.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
void
xpcc::Ads7843<Spi, Cs, Int>::initialize()
{

}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
bool
xpcc::Ads7843<Spi, Cs, Int>::getAverage(uint16_t * buffer, int16_t & value)
{
	uint16_t temp[3];
	temp[0] = (buffer[0] + buffer[1] + buffer[2]) / 3;
	temp[1] = (buffer[3] + buffer[4] + buffer[5]) / 3;
	temp[2] = (buffer[6] + buffer[7] + buffer[8]) / 3;

	uint16_t m0 = abs(temp[0] - temp[1]);
	uint16_t m1 = abs(temp[1] - temp[2]);
	uint16_t m2 = abs(temp[2] - temp[0]);

	if (m0 > threshold && m1 > threshold && m2 > threshold) {
		return false;
	}

	if (m0 < m1)
	{
		if (m2 < m0) {
			value = (temp[0] + temp[2]) / 2;
		}
		else {
			value = (temp[0] + temp[1]) / 2;
		}
	}
	else if (m2 < m1) {
		value = (temp[0] + temp[2]) / 2;
	}
	else {
		value = (temp[1] + temp[2]) / 2;
	}

	return true;
}

template <typename Spi, typename Cs, typename Int>
bool
xpcc::Ads7843<Spi, Cs, Int>::read(glcd::Point * point)
{
	uint_fast8_t count = 0;
	uint16_t bufferX[9];
	uint16_t bufferY[9];

	do
	{
		bufferX[count] = readX();
		bufferY[count] = readY();
		count++;
	} while (!Int::read() && count < 9);

	if (count == 9) {
		glcd::Point p;
		if (getAverage(bufferX, p.x) && getAverage(bufferY, p.y)) {
			*point = p;
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
uint16_t
xpcc::Ads7843<Spi, Cs, Int>::readData(uint8_t command)
{
	Cs::reset();
	xpcc::delayMicroseconds(1);	// xpcc::delay_ns(100);
	Spi::transferBlocking(command);
	xpcc::delayMicroseconds(1);

	uint16_t temp = Spi::transferBlocking(0x00);
	temp <<= 8;
	xpcc::delayMicroseconds(1);

	temp |= Spi::transferBlocking(0x00);
	temp >>= 3;

	Cs::set();

	return (temp & 0xfff);
}
