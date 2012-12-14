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

#ifndef XPCC__LED_LED_HPP
#define XPCC__LED_LED_HPP

#include <stdint.h>
#include "tables.hpp"

namespace xpcc
{
	namespace led
	{
		/**
		 * \brief LED Interface.
		 *
		 * This class supplies a basic interface to fade an LED to any
		 * brightness level.
		 * A lookup-table should be used to store the alpha corrected PWM
		 * values, so that no calculation has to be done at run time.
		 *
		 * \author	Niklas Hauser
		 * \ingroup led
		 */
		class Led
		{
		public:
#ifdef __DOXYGEN__
			/// the constructor must not take any arguments.
			Led();
#endif
			
			/// \param brightness of the LED between 0 and length of lookup-table
			void
			setBrightness(uint16_t brightness);
			
			/// \return brightness of the LED
			uint16_t
			getBrightness();
			
			/// \return \c true if LED is currently fading to another brightness,
			///			\c false if otherwise
			bool
			isFading();
			
			/// Fade from the current brightness to a new brightness in the specified ms.
			void
			fadeTo(uint16_t time, uint16_t brightness);
			
			/**
			 * Mimmics the behaviour of normal lamps, which take a small amount
			 * of time until achiving full brightness.
			 * \param time	specify the fade up time in ms, 0 turn the LED on instantly
			 */
			void
			on(uint16_t time=7);
			
			/**
			 * Mimmics the behaviour of normal lamps, which take a small amount
			 * of time until fully extinguishing.
			 * \param time	specify the fade up time in ms, 0 turn the LED off instantly
			 */
			void
			off(uint16_t time=7);
		};
	}
}

#endif	// XPCC__LED_LED_HPP
