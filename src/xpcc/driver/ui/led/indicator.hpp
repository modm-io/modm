// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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

#ifndef XPCC__LED_INDICATOR_HPP
#define XPCC__LED_INDICATOR_HPP

#include <stdint.h>
#include "led.hpp"

namespace xpcc
{
	namespace led
	{
		/**
		 * \brief LED Indicator.
		 *
		 * This class provided smooth on and off phases, much like a
		 * blinker/indicator on cars.
		 *
		 * \see		Led
		 * \author	Niklas Hauser
		 * \ingroup led
		 * 
		 * \tparam	LedClass	typedef of a Led
		 */
		class Indicator
		{
		private:
			Led* led;
			uint16_t const on;
			uint16_t const off;
			uint8_t const onFade;
			uint8_t const offFade;
			uint8_t counter;
			bool blinkDirection;
			bool isBlinking;
			bool isCounting;
			
			xpcc::Timeout<> timer;
			
		public:
			/**
			 * \param	period		indication cycle period in ms
			 * \param	dutyCycle	on part of the period in percent
			 * \param	onFade		time in ms until the LED is fully on
			 * \param	offFade		time in ms until the LED is fully off
			 */
			Indicator(Led* led, uint16_t const period=1000, float const dutyCycle=0.45f,
					  uint8_t const onFade=75, uint8_t const offFade=110)
			:	led(led), on(period * dutyCycle), off(period - on), onFade(onFade), offFade(offFade),
			counter(0), blinkDirection(true), isBlinking(false), isCounting(false)
			{
			}
			
			/// start indicating for ever
			ALWAYS_INLINE void
			start()
			{
				isBlinking = true;
			}
			
			/// Stops indicating after finishing the current cycle
			ALWAYS_INLINE void
			stop()
			{
				isBlinking = false;
			}
			
			/// Indicate a number of times and then stop
			inline void
			indicate(uint8_t times=1)
			{
				if (times) {
					counter = times;
					isBlinking = true;
					isCounting = true;
				}
			}
			
			/// Must be called at least every ms
			void
			run()
			{
				led->run();
				
				if (timer.isExpired() && (isBlinking || !blinkDirection))
				{
					if (blinkDirection)
					{
						led->on(onFade);
						
						if (isCounting && !--counter) {
							isBlinking = false;
							isCounting = false;
						}
						
						timer.restart(on);
					}
					else
					{
						led->off(offFade);
						
						timer.restart(off);
					}
					blinkDirection = !blinkDirection;
				}
			}
		};
	}
}

#endif	// XPCC__LED_INDICATOR_HPP
