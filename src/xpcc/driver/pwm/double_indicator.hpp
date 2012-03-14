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

#ifndef XPCC__PWM_DOUBLE_INDICATOR_HPP
#define XPCC__PWM_DOUBLE_INDICATOR_HPP

#include "led.hpp"
#include <stdint.h>

namespace xpcc
{
	namespace pwm
	{
		/**
		 * \brief PWM LED Double Indicator.
		 *
		 * \see Led
		 *
		 * \author	Niklas Hauser
		 * \ingroup pwm
		 */
		class DoubleIndicator
		{
		private:
			xpcc::Timeout<> timer;
			Led* led;
			const uint16_t on1;
			const uint16_t pause1;
			const uint16_t on2;
			const uint16_t pause2;
			const uint8_t onFade;
			const uint8_t offFade;
			uint8_t counter;
			bool isBlinking;
			bool isCounting;
			
			enum
			{
				FIRST_FLASH,
				FIRST_BREAK,
				SECOND_FLASH,
				SECOND_BREAK
			} state;
			
		public:
			/**
			 * \param	led			a PWM Led module
			 * \param	period		of the indicating cycle
			 * \param	on1			length of the first "on" part in percent
			 * \param	pause		length of the first "off" part in percent
			 * \param	on2			length of the second "on" part in percent
			 * \param	onFade		time in ms until the LED is fully on
			 * \param	offFade		time in ms until the LED is fully off
			 */
			DoubleIndicator(Led* led, uint16_t period, float on1, float pause,
							float on2, uint8_t onFade=75,uint8_t offFade=125);
			
			/// start indicating for ever.
			void
			start();
			
			/// Stops indicating after finishing the current cycle.
			void
			stop();
			
			/// Indicate a number of times and then stop.
			void
			indicateTimes(uint8_t);
			
			/// Must be called periodically, preferably in the main while loop.
			void
			run();
		};
	}
}

#include "double_indicator_impl.hpp"

#endif	// XPCC__PWM_DOUBLE_INDICATOR_HPP
