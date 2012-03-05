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
 * $Id: double_indicator.hpp 753 2012-03-04 22:29:10Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__LED_DOUBLE_INDICATOR_HPP
#define XPCC__LED_DOUBLE_INDICATOR_HPP

#include <stdint.h>
#include "led.hpp"

namespace xpcc
{
	namespace led
	{
		/**
		 * \brief LED Double Indicator
		 *
		 * \author	Niklas Hauser
		 * \ingroup led
		 * 
		 * \tparam LedClass typedef of a Led
		 */
		template< typename LedClass >
		class DoubleIndicator
		{
		private:
			xpcc::Timeout<> timer;
			LedClass led;
			uint16_t const on1;
			uint16_t const pause1;
			uint16_t const on2;
			uint16_t const pause2;
			uint8_t const onFade;
			uint8_t const offFade;
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
			 * \param	period		of the indicating cycle
			 * \param	on1			length of the first "on" part in percent
			 * \param	pause		length of the first "off" part in percent
			 * \param	on2			length of the second "on" part in percent
			 * \param	onFade		time in ms until the LED is fully on
			 * \param	offFade		time in ms until the LED is fully off
			 */
			DoubleIndicator(uint16_t const period=1000, float const on1=0.1f,
							float const pause=0.2f, float const on2=0.1f,
							uint8_t const onFade=60, uint8_t const offFade=90);
			
			/// start indicating for ever
			void
			start();
			
			/// Stops indicating after finishing the current cycle
			void
			stop();
			
			/// Indicate a number of times and then stop
			void
			indicate(uint8_t times=1);
			
			/// Must be called at least every ms
			void
			run();
		};
	}
}

#include "double_indicator_impl.hpp"

#endif	// XPCC__LED_DOUBLE_INDICATOR_HPP
