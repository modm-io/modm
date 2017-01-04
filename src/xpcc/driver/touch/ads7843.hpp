/*
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, Daniel Krebs
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ADS7843_HPP
#define XPCC_ADS7843_HPP

#include <stdint.h>
#include <modm/ui/display/graphic_display.hpp>

namespace xpcc
{
	/**
	 * ADS7843 Touch Screen Controller.
	 *
	 * The ADS7843 by Texas Instruments is the de-facto standard for cheap
	 * resistive touch screens.
	 *
	 * There are many compatible devices from other manufacturers available,
	 * such as the UH7843 by Zilltek, the TSC2046 and the XPT2046 by XPTEK.
	 * All of these are 100% compatible with the ADS7843.
	 *
	 * @author	Fabian Greif <fabian.greif@rwth-aachen.de>
	 * @ingroup driver_touch
	 */
	template <typename Spi, typename Cs, typename Int>
	class Ads7843
	{
	public:
		static void
		initialize();

		/**
		 * Get the smoothed (x,y) position.
		 *
		 * @param	point
		 * 		(x,y) position on the pressed touchscreen
		 *
		 * @return	`true` if the touchscreen is pressed and the value is
		 * 			stable enough to provide a reading, otherwise `false`.
		 */
		static bool
		read(xpcc::glcd::Point * point);

		static inline uint16_t
		readX()
		{
			return readData(CHX);
		}

		static inline uint16_t
		readY()
		{
			return readData(CHY);
		}

	private:
		static const uint8_t CHX = 0x90;
		static const uint8_t CHY = 0xd0;

		static const uint16_t threshold = 2;

		static bool
		getAverage(uint16_t * buffer, int16_t & value);

		static uint16_t
		readData(uint8_t command);
	};
}

#include "ads7843_impl.hpp"

#endif // XPCC_ADS7843_HPP
