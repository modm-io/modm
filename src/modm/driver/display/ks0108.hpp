/*
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_KS0108_HPP
#define MODM_KS0108_HPP

#include <modm/architecture/interface/delay.hpp>
#include <modm/ui/display/monochrome_graphic_display_buffered_vertical.hpp>

namespace modm
{
	/**
	 * \brief	Driver for KS0108 based LC-displays
	 *
	 * One KS0108 can control 64x64 pixels. Therefore two chips are needed
	 * for a normal 128x64 pixel display. One chip controls the left half, the
	 * other the right half (see PIN_CS1 and PIN_CS2).
	 *
	 * \tparam	E		Enable Pin
	 * \tparam	RW		Read/Write-Select Pin (Low = write, High = read)
	 * \tparam	RS		Command/Data-Select Pin
	 * 					(Low = instruction, High = data)
	 * \tparam	PIN_CS1	Chip-Select 1
	 * \tparam	PIN_CS2	Chip-Select 2
	 * \tparam	PORT	Needs to be a 8-Bit Port. See modm::gpio::Port() or
	 * 					GPIO_PORT() for more details.
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_driver_ks0108
	 */
	template < typename E, typename RW, typename RS,
			   typename PIN_CS1, typename PIN_CS2, typename PORT >
	class Ks0108 : public MonochromeGraphicDisplayBufferedVertical<128, 64>
	{
	public:
		/**
		 * \brief	Initialize the display
		 */
		void
		initialize();

		/**
		 * \brief	Update the display with the content of the RAM buffer
		 */
		virtual void
		update();

	protected:
		// Timing constants form the datasheet
		static constexpr float DATA_SET_UP_TIME = 0.14f;
		static constexpr float MIN_E_HIGH_TIME = 0.45f;
		static constexpr float MIN_E_LOW_TIME = 0.45f;

	protected:
		/// Write one byte (Sets RW and E)
		void
		writeByte(uint8_t data);

		/// Read one byte (Sets RW and E)
		uint8_t
		readByte();

		/**
		 * \brief	Wait until the busy flag is cleared
		 *
		 * To avoid a complete blocking of the microcontroller the waiting
		 * will be aborted after approximately 60µs.
		 */
		void
		waitBusy();

		/// Sets RS (= data) and calls writeByte()
		void
		writeData(uint8_t data);

		/// Clears RS (= instruction) and calls writeByte()
		void
		writeCommand(uint8_t command);

		inline void
		selectLeftChip()
		{
			cs1.set();
			cs2.reset();
		}

		inline void
		selectRightChip()
		{
			cs1.reset();
			cs2.set();
		}

		E e;
		RW rw;
		RS rs;
		PIN_CS1 cs1;
		PIN_CS2 cs2;
		PORT port;
	};
}

#include "ks0108_impl.hpp"

#endif // MODM_KS0108_HPP
