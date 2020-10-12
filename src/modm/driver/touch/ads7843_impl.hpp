/*
 * Copyright (c) 2009-2010, 2012-2013, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, Daniel Krebs
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2020, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ADS7843_HPP
#	error "Don't include this file directly, use 'ads7843.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
void
modm::Ads7843<Spi, Cs, Int>::initialize()
{

}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
uint16_t
modm::Ads7843<Spi, Cs, Int>::getBestTwo(uint16_t *buf)
{
	uint16_t value;
	uint16_t m0 = abs(buf[0] - buf[1]);
	uint16_t m1 = abs(buf[1] - buf[2]);
	uint16_t m2 = abs(buf[2] - buf[0]);

	if (m0 < m1)
	{
		if (m2 < m0) {
			value = (buf[0] + buf[2]) / 2;
		}
		else {
			value = (buf[0] + buf[1]) / 2;
		}
	}
	else if (m2 < m1) {
		value = (buf[0] + buf[2]) / 2;
	}
	else {
		value = (buf[1] + buf[2]) / 2;
	}

	return value;
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
bool
modm::Ads7843<Spi, Cs, Int>::read(glcd::Point * point)
{
	uint16_t z1 = readData(CHZ1);
	uint16_t z2 = readData(CHZ2);
	uint16_t z = z1 + 4095 - z2;
	uint16_t xbuf[3];
	uint16_t ybuf[3];

	if(z > threshold) {
		xbuf[0] = readData(CHX);
		ybuf[0] = readData(CHY);
		xbuf[1] = readData(CHX);
		ybuf[1] = readData(CHY);
		xbuf[2] = readData(CHX);
		ybuf[2] = readData(CHY);

		point->x = (int16_t)getBestTwo(xbuf);
		point->y = (int16_t)getBestTwo(ybuf);
		return true;
	} else {
		return false;
	}
}

// ----------------------------------------------------------------------------
template <typename Spi, typename Cs, typename Int>
uint16_t
modm::Ads7843<Spi, Cs, Int>::readData(uint8_t command)
{
	Cs::reset();
	modm::delay_us(1);	// modm::delay_ns(100);
	Spi::transferBlocking(command);
	modm::delay_us(1);

	uint16_t temp = Spi::transferBlocking(0x00);
	temp <<= 8;
	modm::delay_us(1);

	temp |= Spi::transferBlocking(0x00);
	temp >>= 3;

	Cs::set();

	return (temp & 0xfff);
}
