/*
 * Copyright (c) 2013, Daniel Krebs
 * Copyright (c) 2013, Fabian Greif
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PARALLEL_TFT_HPP
#define MODM_PARALLEL_TFT_HPP

#include <modm/architecture/driver/delay.hpp>
#include <modm/ui/display/graphic_display.hpp>

namespace modm
{
	/**
	 * TFT display connected to a 16 bit parallel bus
	 *
	 * Supports (among others) the following displays:
	 * - WaveShare 3,2" TFT (Model B), Controller SSD1289
	 *
	 * @author	Fabian Greif
	 * @ingroup	driver_display
	 */
	template <typename INTERFACE>
	class ParallelTft : public GraphicDisplay
	{
	public:
		ParallelTft(INTERFACE& interface);

		void
		initialize();

		// see GraphicDisplay::getWidth()
		virtual uint16_t
		getWidth() const;

		// see GraphicDisplay::getHeight()
		virtual uint16_t
		getHeight() const;

		// set GraphicDisplay::clear()
		virtual void
		clear();

		/**
		 * Not used here as all operations are performed directly
		 * on the display.
		 */
		virtual void
		update()
		{
		}

	private:
		enum class Device
		{
			ILI9320,	// device code = 0x9320
			ILI9325,	// device code = 0x9325
			ILI9328,	// device code = 0x9328
			ILI9331,	// device code = 0x9331
			SSD1298,	// device code = 0x8999
			SSD1289,	// device code = 0x8989
			ST7781,		// device code = 0x7783
			LGDP4531,	// device code = 0x4531
			SPFD5408B,	// device code = 0x5408
			R61505U,	// device code = 0x1505 or 0x0505
			//HX8347D,	// device code = 0x0047
			//HX8347A,	// device code = 0x0047
			LGDP4535,	// device code = 0x4535
			//SSD2119,	// 3.5 LCD, device code = 0x9919
		};

		virtual void
		setPixel(int16_t x, int16_t y);

		virtual void
		clearPixel(int16_t x, int16_t y);

		virtual bool
		getPixel(int16_t x, int16_t y);

		void
		writeCursor(uint16_t x, uint16_t y);

		void
		writeRegister(uint16_t reg, uint16_t value);

		INTERFACE& interface;
		Device deviceCode;
	};
}

#include "parallel_tft_impl.hpp"

#endif // MODM_PARALLEL_TFT_HPP
