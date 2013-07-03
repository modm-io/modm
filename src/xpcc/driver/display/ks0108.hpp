// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__KS0108_HPP
#define XPCC__KS0108_HPP

#include <xpcc/architecture/driver/delay.hpp>

#include <xpcc/ui/display/buffered_graphic_display.hpp>

namespace xpcc
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
	 * \tparam	PORT	Needs to be a 8-Bit Port. See xpcc::gpio::Port() or
	 * 					GPIO__PORT() for more details.
	 * 
	 * \author	Fabian Greif
	 * \ingroup	lcd
	 */
	template < typename E, typename RW, typename RS,
			   typename PIN_CS1, typename PIN_CS2, typename PORT >
	class Ks0108 : public BufferedGraphicDisplay<128, 64>
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

#endif // XPCC__KS0108_HPP
