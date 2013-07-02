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

#ifndef XPCC__LED_TLC594X_MULTIPLE_LED_HPP
#define XPCC__LED_TLC594X_MULTIPLE_LED_HPP

#include <stdint.h>
#include "led.hpp"
#include "tables.hpp"
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
		 */
		template< typename PwmController >
		class TLC594XMultipleLed : public Led
		{
		private:
			uint16_t currentValue;
			
			float deltaValue;
			float startValue;
			uint16_t endValue;
			uint16_t fadeTime;
			
			xpcc::PeriodicTimer<> timer;
			
			const uint8_t *channels;
			uint8_t channelSize;
			
			xpcc::accessor::Flash<uint16_t> table;
			std::size_t const tableSize;
			
			void
			setChannels(uint16_t brightness)
			{
				for (uint_fast8_t i = 0; i < channelSize; ++i)
				{
					PwmController::setChannel(channels[i], table[brightness]);
				}
			}
			
		public:
			TLC594XMultipleLed(const uint8_t* channels, uint8_t const channelSize, const uint16_t* table=led::table12_256, uint16_t const tableSize=256)
			:	currentValue(0), deltaValue(0), startValue(0), endValue(0), fadeTime(0), timer(1),
				channels(channels), channelSize(channelSize), table(table), tableSize(tableSize)
			{
				setChannels(0);
			}
			
			virtual void
			setBrightness(uint16_t brightness)
			{
				fadeTime = 0;
				if (brightness > tableSize-1) brightness = tableSize-1;
				currentValue = brightness;
				
				setChannels(currentValue);
			}
			
			virtual uint16_t
			getBrightness()
			{
				return currentValue;
			}
			
			virtual bool
			isFading()
			{
				return static_cast<bool>(fadeTime);
			}
			
			virtual void
			fadeTo(uint16_t time, uint16_t brightness)
			{
				if (brightness == currentValue) return;
				if (brightness > tableSize-1) brightness = tableSize-1;
				if (!time) {
					currentValue = brightness;
					setChannels(currentValue);
				}
				else {
					startValue = currentValue;
					endValue = brightness;
					deltaValue = (endValue - startValue) / static_cast<float>(time);
				}
				fadeTime = time;
			}
			
			virtual void
			on(uint16_t time=7)
			{
				fadeTo(time, tableSize-1);
			}
			
			virtual void
			off(uint16_t time=7)
			{
				fadeTo(time, 0);
			}
			
			/// must be called at least every ms
			void
			run()
			{
				if (timer.isExpired() && fadeTime)
				{
					startValue += deltaValue;
					currentValue = static_cast<uint16_t>(startValue);
					if (!--fadeTime) currentValue = endValue;
					
					setChannels(currentValue);
				}
			}
		};
	}
}

#endif	// XPCC__LED_TLC594X_MULTIPLE_LED_HPP
