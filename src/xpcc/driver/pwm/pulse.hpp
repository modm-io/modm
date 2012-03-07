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
 *
 * $Id: pulse.hpp 612 2011-10-01 21:27:09Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__PWM_PULSE_HPP
#define XPCC__PWM_PULSE_HPP

#include "led.hpp"
#include <stdint.h>

namespace xpcc
{
	namespace pwm
	{
		/**
		 * \brief PWM LED Pulser.
		 *
		 * \see Led
		 *
		 * \author	Niklas Hauser
		 * \ingroup pwm
		 */
		class Pulse
		{
		private:
			Led* led;
			const uint16_t halfPeriod;
			uint8_t counter;
			bool pulseDirection;
			bool isPulsing;
			bool isCounting;
			
		public:
			/**
			 * \param	led		a PWM Led module
			 * \param	period	pulse cycle period in ms.
			 */
			Pulse(Led* led, uint16_t period=1000);
			
			/// start pulsing for ever.
			void
			start();
			
			/// Stops pulsing after finishing the current cycle.
			void
			stop();
			
			/// Pulse a number of times and then stop.
			void
			pulseTimes(uint8_t);
			
			/// Must be called periodically, preferably in the main while loop.
			void
			run();
		};
	}
}

#include "pulse_impl.hpp"

#endif	// XPCC__PWM_PULSE_HPP
