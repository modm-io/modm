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

#ifndef XPCC__LED_PULSE_HPP
#define XPCC__LED_PULSE_HPP

#include <stdint.h>
#include "led.hpp"

namespace xpcc
{
	namespace led
	{
		/**
		 * \brief LED Pulser
		 *
		 * This class fades an LED up and down during the period.
		 *
		 * \see		Led
		 * \author	Niklas Hauser
		 * \ingroup led
		 * 
		 * \tparam LedClass typedef of a Led
		 */
		class Pulse
		{
		private:
			Led* led;
			uint16_t const halfPeriod;
			uint8_t counter;
			bool pulseDirection;
			bool isPulsing;
			bool isCounting;
			
		public:
			/// \param	period	pulse cycle period in ms.
			Pulse(Led* led, uint16_t const period=1000)
			:	led(led), halfPeriod(period/2), counter(0),
				pulseDirection(false), isPulsing(false), isCounting(false)
			{
			}
			
			/// start pulsing forever
			ALWAYS_INLINE void
			start()
			{
				isPulsing = true;
			}
			
			/// Stops pulsing after finishing the current cycle
			ALWAYS_INLINE void
			stop()
			{
				isPulsing = false;
			}
			
			/// Pulses a number of times and then stops
			inline void
			pulse(uint8_t times=1)
			{
				if (times) {
					counter = times;
					isPulsing = true;
					isCounting = true;
				}
				else isPulsing = false;
			}
			
			/// Must be called at least every ms
			void
			run()
			{
				led->run();
				
				if (!led->isFading() && (isPulsing || !pulseDirection))
				{
					if (pulseDirection) {
						led->on(halfPeriod);
						
						if (isCounting && !--counter) {
							isPulsing = false;
							isCounting = false;
						}
					}
					else {
						led->off(halfPeriod);
					}
					pulseDirection = !pulseDirection;
				}
			}
		};
	}
}

#endif	// XPCC__LED_PULSE_HPP
