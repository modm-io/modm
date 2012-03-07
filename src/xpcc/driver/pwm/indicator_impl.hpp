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
 * $Id: indicator_impl.hpp 612 2011-10-01 21:27:09Z salkinium $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__PWM_INDICATOR_HPP
#	error	"Don't include this file directly, use 'indicator.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
xpcc::pwm::Indicator::Indicator(Led* led, uint16_t period, const float dutyCycle,
							const uint8_t onFade, const uint8_t offFade)
:	led(led), on(period * dutyCycle), off(period - on), onFade(onFade), offFade(offFade),
	counter(0), blinkDirection(true), isBlinking(false), isCounting(false)
{
}

void
xpcc::pwm::Indicator::start()
{
	isBlinking = true;
}

void
xpcc::pwm::Indicator::stop()
{
	isBlinking = false;
}

void
xpcc::pwm::Indicator::indicateTimes(uint8_t times)
{
	if (times) {
		counter = times;
		isBlinking = true;
		isCounting = true;
	}
}

void
xpcc::pwm::Indicator::run()
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
// ----------------------------------------------------------------------------
