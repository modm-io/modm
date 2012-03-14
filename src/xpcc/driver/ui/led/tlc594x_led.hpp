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

#ifndef XPCC__LED_TLC594X_LED_HPP
#define XPCC__LED_TLC594X_LED_HPP

#include <stdint.h>
#include "led.hpp"
#include "led_table.hpp"
#include <xpcc/driver/pwm/tlc594x.hpp>
#include <xpcc/workflow/periodic_timer.hpp>
#include <xpcc/architecture/driver/accessor/flash.hpp>

namespace xpcc
{
	namespace led
	{
		/**
		 * \brief LED Implementation for the TLC594X.
		 *
		 * \author	Niklas Hauser
		 * \ingroup led
		 *
		 * \tparam	PwmController	TLC594X driver
		 * \tparam	CHANNEL			the channel the LED is attached to
		 * \tparam	PwmTable		the pwm value look-up table
		 * \tparam	PwmTableSize	the size of the look-up table
		 */
		template< typename PwmController,
				  uint16_t CHANNEL,
				  const uint16_t* PwmTable=PwmTable12_256,
				  uint16_t const PwmTableSize=256 >
		class TLC594XLed : public Led
		{
		private:
			uint16_t currentValue;
			
			float deltaValue;
			float startValue;
			uint16_t endValue;
			uint16_t fadeTime;
			
			xpcc::PeriodicTimer<> timer;
			xpcc::accessor::Flash<uint16_t> table;
			
		public:
			TLC594XLed();
			
			void
			setBrightness(uint16_t brightness);
			
			uint16_t
			getBrightness();
			
			bool
			isFading();
			
			void
			fadeTo(uint16_t time, uint16_t brightness);
			
			void
			on(uint16_t time=7);
			
			void
			off(uint16_t time=7);
			
			/// must be called at least every ms
			void
			run();
		};
	}
}

#include "tlc594x_led_impl.hpp"

#endif	// XPCC__LED_TLC594X_LED_HPP
