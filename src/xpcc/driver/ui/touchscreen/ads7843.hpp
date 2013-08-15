// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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

#ifndef XPCC__ADS7843_HPP
#define XPCC__ADS7843_HPP

#include <stdint.h>
#include <xpcc/driver/ui/display/graphic_display.hpp>

namespace xpcc
{
	/**
	 * ADS7843 Touch Screen Controller.
	 * 
	 * The ADS7843 by Texas Instruments is the de-facto standard for cheap
	 * resistive touch screens.
	 * 
	 * There are many compatible devices from other manufacturers available,
	 * such as the UH7843 by Zilltek, the TSC2046 and the XPT2046 by XPTEK.
	 * All of these are 100% compatible with the ADS7843. 
	 * 
	 * @author	Fabian Greif <fabian.greif@rwth-aachen.de>
	 */
	template <typename Spi, typename Cs, typename Int>
	class Ads7843
	{
	public:
		static void
		initialize();
		
		/**
		 * Get the smoothed (x,y) position.
		 * 
		 * @param	point
		 * 		(x,y) position on the pressed touchscreen
		 * 
		 * @return	`true` if the touchscreen is pressed and the value is
		 * 			stable enough to provide a reading, otherwise `false`.
		 */
		static bool
		read(xpcc::glcd::Point * point);
		
		static inline uint16_t
		readX()
		{
			return readData(CHX);
		}
		
		static inline uint16_t
		readY()
		{
			return readData(CHY);
		}
		
	private:
		static const uint8_t CHX = 0x90;
		static const uint8_t CHY = 0xd0;
		
		static const uint16_t threshold = 2;
		
		static bool
		getAverage(uint16_t * buffer, int16_t & value);
		
		static uint16_t
		readData(uint8_t command);
	};
}

#include "ads7843_impl.hpp"

#endif // XPCC__ADS7843_HPP
