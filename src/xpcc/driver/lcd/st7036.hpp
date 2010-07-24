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

#ifndef XPCC__ST7036_HPP
#define XPCC__ST7036_HPP

#include <xpcc/driver/lcd/lcd.hpp>

#include <xpcc/architecture/general/accessor/flash.hpp>
#include <xpcc/architecture/general/time/delay.hpp>

namespace xpcc
{
	/**
	 * \brief	Driver for ST7036 based LC-displays
	 * 
	 * \todo	documentation
	 * \todo	make this class adaptable to other voltages and line counts!
	 * 
	 * \see		Lcd
	 * 
	 * \author	Fabian Greif
	 * \ingroup	driver
	 */
	template <typename SPI, typename CS, typename RS>
	class St7036 : public Lcd
	{
	public:
		/// Constructor
		St7036();
		
		/**
		 * \brief	Initialize the display
		 * 
		 * The display needs some time to initalize after startup. You have
		 * to wait at least 50 ms until calling this method.
		 */
		virtual void
		initialize();
		
		virtual void
		writeRaw(char c);
		
		//virtual void
		//command(Command command);
		
		virtual void
		setPosition(uint8_t line, uint8_t column);
		
		// TODO
		//void
		//setContrast();
	
	protected:
		void
		writeCommand(uint8_t command);
	};
}

#include "st7036_impl.hpp"

#endif // XPCC__ST7036_HPP
