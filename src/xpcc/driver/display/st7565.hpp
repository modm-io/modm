/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ST7565_HPP
#define XPCC__ST7565_HPP

#include <modm/architecture/driver/accessor/flash.hpp>
#include <modm/architecture/driver/delay.hpp>

#include <modm/ui/display/buffered_graphic_display.hpp>

namespace xpcc
{
	/**
	 * \brief	Driver for ST7565 based LC-displays
	 *
	 * \author	Fabian Greif
	 * \ingroup	driver_display
	 */
	template <typename SPI, typename CS, typename A0, typename Reset,
			  unsigned int Width, unsigned int Height, bool TopView>
	class St7565 : public BufferedGraphicDisplay<Width, Height>
	{
	public:
		virtual ~St7565()
		{
		}

		/**
		 * \brief	Update the display with the content of the RAM buffer
		 */
		virtual void
		update();

		/// Invert the display content
		void
		setInvert(bool invert);

	protected:
		xpcc_always_inline void
		initialize(xpcc::accessor::Flash<uint8_t> configuration, uint8_t size);

		SPI spi;
		CS cs;
		A0 a0;
		Reset reset;
	};
}

#include "st7565_impl.hpp"

#endif // XPCC__ST7565_HPP
