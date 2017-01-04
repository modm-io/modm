/*
 * Copyright (c) 2009-2013, Fabian Greif
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2012-2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SIEMENS_S75_HPP
#define XPCC_SIEMENS_S75_HPP

#include <modm/architecture/driver/delay.hpp>
#include <modm/driver/bus/bitbang_memory_interface.hpp>

#include <modm/ui/display/buffered_graphic_display.hpp>

namespace xpcc
{
	/**
	 * \brief	Driver for Siemens S75/CX75/C81/M75 displays
	 *
	 * The controller in displays manufactured by Alps is a
	 * SOLOMON SYSTECH SSD1286 which can drive color
	 * displays of up to 132 x 176 pixels in 18 bit colour.
	 *
	 * The portrait mode is a bit more 'native' for this display because
	 * xpcc::BufferedGraphicDisplay requests that the vertical resolution is
	 * dividable by 8.
	 *
	 * In portrait mode the connector is at top.
	 * In landscapeLeft  mode the connector is at the left border.
	 * In landscapeRight mode the connector is at the right border.
	 *
	 * Pinout at LCD:
	 *  1  IN  CD         Command / Data                      Low = Command (Register) High = Data.
	 *  2  IN  RESET      Resets the display.  Active low.    Low = Reset,             High = No reset.
	 *  3  IN  CS         Chip select.         Active low.    Low = Chip is selected,  High = Chip is not selected.
	 *  4  OUT VD         Sync Output.         Unused.
	 *  5      GND        Supply
	 *  6  IN  D0         Bit 0 of parallel data
	 *  7      3V3 (2V9)  Supply
	 *  8      GND        Supply
	 *  9      VLED +     Supply Backlight
	 * 10      VLED -     Supply Backlight
	 * 11      GND        Supply
	 * 12  IN  RD         Read Strobe.         Must be tied high.
	 * 13  IN  WR         Write Strobe.        High-to-Low strobe write data to display memory.
	 * 14  IN  D1         Bit 1 of parallel data.
	 * 15  IN  D2         Bit 2 of parallel data.
	 * 16  IN  D3         Bit 3 of parallel data.
	 * 17  IN  D4         Bit 4 of parallel data.
	 * 18  IN  D5         Bit 5 of parallel data.
	 * 19  IN  D6         Bit 6 of parallel data.
	 * 20  IN  D7         Bit 7 of parallel data.
	 *
	 * Pin 1 is unmarked. Pin 5 can be recognised by a thicker trace which is GND.
	 *
	 * The backlight (VLED +, VLED -) consists of four white
	 * LEDs in series. The forward voltage is about 12 volts.
	 *
	 * \ingroup	driver_display
	 */

	enum class Orientation : uint8_t
	{
		Portrait, 				//< Connector top
		LandscapeRight,			//< Connector right
		LandscapeLeft,			//< Connector left
		PortraitUpsideDown,		//< Connector bottom
	};

	template <
		typename MEMORY,
		typename RESET,
		uint16_t  WIDTH,
		uint16_t  HEIGHT,
		xpcc::Orientation ORIENTATION>
	class SiemensS75Common :
			public BufferedGraphicDisplay<WIDTH, HEIGHT>
	{
	public:
		SiemensS75Common(MEMORY& interface) :
			interface(interface)
		{
		}

		void
		update(void);

		void
		initialize(void);

	protected:
		xpcc_always_inline void
		lcdCls(const uint16_t colour);

		xpcc_always_inline void
		lcdSettings();

	private:
		MEMORY& interface;
	};

	template <typename MEMORY, typename RESET>
	class SiemensS75Portrait :
		public SiemensS75Common<MEMORY, RESET, 136, 176, xpcc::Orientation::Portrait>
	{
	};

	template <typename MEMORY, typename RESET>
	class SiemensS75PortraitUpsideDown:
		public SiemensS75Common<MEMORY, RESET, 136, 176, xpcc::Orientation::PortraitUpsideDown>
	{
	};

	/**
	 * The display in landscape mode does not match the required
	 * alignment of BufferedGraphicDisplay which requests that
	 * the vertical resolution can be divided by 8.
	 *
	 */
	template <typename MEMORY, typename RESET>
	class SiemensS75LandscapeLeft :
		public SiemensS75Common<MEMORY, RESET, 176, 136, xpcc::Orientation::LandscapeLeft>
	{
	public:
		explicit SiemensS75LandscapeLeft(MEMORY& interface) :
		SiemensS75Common<MEMORY, RESET, 176, 136, xpcc::Orientation::LandscapeLeft>(interface) { }
	};

	template <typename MEMORY, typename RESET>
	class SiemensS75LandscapeRight :
		public SiemensS75Common<MEMORY, RESET, 176, 136, xpcc::Orientation::LandscapeRight>
	{
	public:
		explicit SiemensS75LandscapeRight(MEMORY& interface) :
		SiemensS75Common<MEMORY, RESET, 176, 136, xpcc::Orientation::LandscapeRight>(interface) { }
	};
}

#include "siemens_s75_impl.hpp"

#endif // XPCC_SIEMENS_S75_HPP
