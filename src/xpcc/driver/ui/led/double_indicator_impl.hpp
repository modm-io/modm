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
 * $Id: double_indicator_impl.hpp 753 2012-03-04 22:29:10Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__LED_DOUBLE_INDICATOR_HPP
#	error	"Don't include this file directly, use 'double_indicator.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template< typename LedClass >
xpcc::led::DoubleIndicator<LedClass>::DoubleIndicator(uint16_t const period, float const on1,
													  float const pause, float const on2,
													  uint8_t const onFade, uint8_t const offFade)
:	on1(period * on1), pause1(period * pause),
	on2(period * on2), pause2(period - on1 - pause1 - on2),
	onFade(onFade), offFade(offFade),counter(0),
	isBlinking(false), isCounting(false), state(FIRST_FLASH)
{
}

template< typename LedClass >
void
xpcc::led::DoubleIndicator<LedClass>::start()
{
	state = FIRST_FLASH;
	isBlinking = true;
}

template< typename LedClass >
void
xpcc::led::DoubleIndicator<LedClass>::stop()
{
	isBlinking = false;
}

template< typename LedClass >
void
xpcc::led::DoubleIndicator<LedClass>::indicate(uint8_t times)
{
	if (times) {
		counter = times;
		isBlinking = true;
		isCounting = true;
	}
}

template< typename LedClass >
void
xpcc::led::DoubleIndicator<LedClass>::run()
{
		led.run();
		
		if (timer.isExpired() && (isBlinking || state == SECOND_BREAK))
		{
			switch (state) {
				case FIRST_FLASH:
					led.on(onFade);
					
					timer.restart(on1);
					state = FIRST_BREAK;
					break;
					
				case FIRST_BREAK:
					led.off(offFade);
					
					timer.restart(pause1);
					state = SECOND_FLASH;
					break;
					
				case SECOND_FLASH:
					led.on(onFade);
					
					if (isCounting && !--counter) {
						isBlinking = false;
						isCounting = false;
					}
					
					timer.restart(on2);
					state = SECOND_BREAK;
					break;
					
				case SECOND_BREAK:
					led.off(offFade);
					
					timer.restart(pause2);
					
				default:
					state = FIRST_FLASH;
					break;
			}
		}
}
// ----------------------------------------------------------------------------
