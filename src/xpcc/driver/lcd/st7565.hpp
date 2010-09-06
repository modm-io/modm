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

#ifndef XPCC__ST7565_HPP
#define XPCC__ST7565_HPP

#include <xpcc/architecture/driver/accessor/flash.hpp>
#include <xpcc/architecture/driver/time/delay.hpp>

namespace xpcc
{
	/**
	 * \brief	Driver for ST7565 based LC-displays
	 * 
	 * 
	 * 
	 * \author	Fabian Greif
	 * \ingroup	driver
	 */
	template <typename SPI, typename CS, typename A0, typename Reset>
	class St7565
	{
	public:
		/**
		 * \brief	Initialize the display
		 * 
		 * The display needs some time to initalize after startup. You have
		 * to wait at least 50 ms until calling this method.
		 */
		void
		initialize();
		
		/**
		 * \brief	Update the display with the content of the RAM buffer
		 */
		void
		update();
		
		// all these functions work on the RAM buffer, only with a call
		// of update() the content is transfered to the display
		void
		clear();
		
		// x = 0..127, y = 0..63
		void
		setPixel(uint8_t x, uint8_t y);
		
		// x = 0..127, y = 0..63
		void
		clearPixel(uint8_t x, uint8_t y);
		
	protected:
		uint8_t buffer[128][8];
		
		SPI spi;
		CS cs;
		A0 a0;
		Reset reset;
	};
}

#include "st7565_impl.hpp"

#endif // XPCC__ST7565_HPP
