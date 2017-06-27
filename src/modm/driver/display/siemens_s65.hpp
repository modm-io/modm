/*
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2013, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SIEMENS_S65_HPP
#define MODM_SIEMENS_S65_HPP

#include <modm/architecture/interface/delay.hpp>

#include <modm/ui/display/buffered_graphic_display.hpp>

// to enable this define add it to your ``project.cfg`` and
// include <modm_config.hpp> before including this file.
#ifndef S65_LPC_ACCELERATED
#	define S65_LPC_ACCELERATED 0
#endif

namespace modm
{
	/**
	 * \brief	Driver for Siemens S65 displays
	 *
	 * The controller is a Hitachi HD66773 which can drive color displays of up
	 * ot 132 x 176 pixels in 18 bit colour.
	 *
	 * The portrait mode is a bit more 'native' for this display because
	 * modm::BufferedGraphicDisplay requests that the vertical resolution is
	 * dividable by 8.
	 *
	 * In portrait mode the connector is at the bottom.
	 * In landscape mode the connector is at the right border.
	 *
	 * \ingroup	driver_display
	 */

	// common for landscape and portrait
	template <typename SPI, typename CS, typename RS, typename Reset>
	class SiemensS65Common
	{
	protected:
		modm_always_inline void
		writeCmd(uint8_t reg, uint16_t param);

		modm_always_inline void
		writeReg(uint8_t reg);

		modm_always_inline void
		writeData(uint16_t param);

		modm_always_inline void
		lcdCls(uint16_t colour);

		modm_always_inline void
		lcdSettings(bool landscape);

	};

	template <typename SPI, typename CS, typename RS, typename Reset>
	class SiemensS65Portrait :
		public BufferedGraphicDisplay<132, 176>,
		public SiemensS65Common<SPI, CS, RS, Reset>
	{
	public:
		void
		initialize();

		/**
		 * \brief	Update the display with the content of the RAM buffer
		 */
		virtual void
		update();

	};

		/**
		 * The display in landscape mode does not match the required
		 * alignment of BufferedGraphicDisplay which requests that
		 * the vertical resolution can be divided by 8.
		 *
		 */
	template <typename SPI, typename CS, typename RS, typename Reset>
	class SiemensS65Landscape :
		public BufferedGraphicDisplay<176, 136>,
		public SiemensS65Common<SPI, CS, RS, Reset>
	{
	public:
		void
		initialize();

		/**
		 * \brief	Update the display with the content of the RAM buffer
		 */
		virtual void
		update();
	};

}

#include "siemens_s65_impl.hpp"

#endif // MODM_SIEMENS_S65_HPP
