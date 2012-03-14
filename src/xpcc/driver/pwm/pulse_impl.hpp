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

#ifndef XPCC__PWM_PULSE_HPP
#	error	"Don't include this file directly, use 'pulse.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
xpcc::pwm::Pulse::Pulse(Led* led, uint16_t period)
:	led(led), halfPeriod(period/2), counter(0),
	pulseDirection(false), isPulsing(false), isCounting(false)
{
}

void
xpcc::pwm::Pulse::start()
{
	isPulsing = true;
}

void
xpcc::pwm::Pulse::stop()
{
	isPulsing = false;
}

void
xpcc::pwm::Pulse::pulseTimes(uint8_t times)
{
	if (times) {
		counter = times;
		isPulsing = true;
		isCounting = true;
	}
}

void
xpcc::pwm::Pulse::run()
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
// ----------------------------------------------------------------------------
