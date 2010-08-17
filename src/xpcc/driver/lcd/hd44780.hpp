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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__HD44780_HPP
#define XPCC__HD44780_HPP

#include <xpcc/driver/lcd/lcd.hpp>
#include <xpcc/architecture/driver/time/delay.hpp>

namespace xpcc
{
	/**
	 * \brief	Driver for HD447800 compatible displays
	 * 
	 * An HD44780 Character LCD is a de facto industry standard liquid crystal
	 * display (LCD) display device. These screens come in common
	 * configurations of 8x1, which is one row of eight characters, 16x2, and
	 * 20x4, among others.
	 * 
	 * This driver uses the 4-Bit mode with read back. Therefore you need to
	 * connect all there control pins (RW, RS, E) and the high nibble of the
	 * data lines.
	 * 
	 * Example:
	 * \include lcd/hd44780/main.cpp
	 * 
	 * \tparam	E		Enable Pin
	 * \tparam	RW		Read/Write-Select Pin
	 * \tparam	RS		Command/Data-Select Pin
	 * \tparam	DATA	Needs to be a Nibble. See Nibble() for more details.
	 * 
	 * \see		Lcd
	 * \see		http://en.wikipedia.org/wiki/HD44780_Character_LCD
	 * 
	 * \author	Fabian Greif
	 * \ingroup	driver
	 */
	template <typename E, typename RW, typename RS, typename DATA>
	class Hd44780 : public Lcd
	{
	public:
		/// Constructor
		Hd44780();
		
		/**
		 * \brief	Initialize the display
		 * 
		 * The display needs some time to initialize after startup. You have
		 * to wait at least 50 ms after startup before calling this method.
		 */
		virtual void
		initialize();
		
		virtual void
		writeRaw(char c);
		
		virtual void
		setPosition(uint8_t line, uint8_t column);
		
		virtual void
		command(Command command);
		
	protected:
		void
		writeNibble(uint8_t data);
		
		uint8_t
		readByte();
		
		/// Wait unitl the busy flag is reseted
		void
		waitBusy();
		
		void
		writeCommand(uint8_t command);
	};
}

#include "hd44780_impl.hpp"

#endif // XPCC__HD44780_HPP
