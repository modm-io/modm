/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_NOKIA6610_HPP
#define MODM_NOKIA6610_HPP

#include <modm/architecture/driver/delay.hpp>

#include <modm/ui/display/buffered_graphic_display.hpp>

namespace modm
{
	/**
	 * \brief	Driver for Nokia6610 displays
	 *
	 * Adapted from www.olimex.com
	 *
	 * bool GE12 = true generates driver for GE12 Philips PCF8833 132 x 132 x 3 driver
	 * bool GE12 = false generates driver for GE8 Epson S1D15G10
	 *
	 * GE12 not implemented yet
	 *
	 * \ingroup	driver_display
	 */
	template <typename SPI, typename CS, typename Reset, bool GE12>
	class Nokia6610 : public BufferedGraphicDisplay<130, 128>
	{
	public:
		void
		initialize();

		/**
		 * \brief	Update the display with the content of the RAM buffer
		 */
		virtual void
		update();


		/**
		 * \brief	Change display contrast 0..127
		 */
		void
		setContrast(uint8_t contrast);

	private:
		modm_always_inline void
		lcdSettings(void);

		modm_always_inline void
		writeSpiCommand(uint16_t data);

		modm_always_inline void
		writeSpiData(uint16_t data);

		modm_always_inline void
		writeSpi9Bit(uint16_t data);

		modm_always_inline void
		writeSpi9BitFlush();

		void
		lcdWrite130x130bmp(void);

		// 9bit spi managing
		uint16_t temp;				//
		uint8_t countValidBits;		// always < 8
	};
}

#include "nokia6610_impl.hpp"

#endif // MODM_NOKIA6610_HPP
