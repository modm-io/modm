/*
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ST7565_HPP
#define MODM_ST7565_HPP

#include <modm/architecture/interface/accessor_flash.hpp>
#include <modm/architecture/interface/delay.hpp>

#include <modm/ui/display/monochrome_graphic_display_buffered_vertical.hpp>

namespace modm
{
	/**
	 * \brief	Driver for ST7565 based LC-displays
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_driver_ea_dog
	 */
	template <typename SPI, typename CS, typename A0, typename Reset,
			  unsigned int Width, unsigned int Height, bool TopView>
	class St7565 : public MonochromeGraphicDisplayBufferedVertical<Width, Height>
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
		modm_always_inline void
		initialize(modm::accessor::Flash<uint8_t> configuration, uint8_t size);

		SPI spi;
		CS cs;
		A0 a0;
		Reset reset;
	};
}

#include "st7565_impl.hpp"

#endif // MODM_ST7565_HPP
