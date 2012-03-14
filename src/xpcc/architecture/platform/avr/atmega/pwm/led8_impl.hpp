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

#ifndef XPCC_ATMEGA__PWM_LED8_HPP
#	error	"Don't include this file directly, use 'led8.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
xpcc::atmega::pwm::Led8::Led8(volatile uint8_t* outputCompare, const float maximum)
:	maximum(256 * maximum - 1), compareRegister(outputCompare), value(0),
fading(false), fadeTime(1), startValue(0), endValue(0)
{
	*compareRegister = ::xpcc::pwm::PwmValues8[0];
}

void
xpcc::atmega::pwm::Led8::setValue(float brightness)
{
	uint8_t fadeValue = brightness * maximum;
	
	setValue(fadeValue);
}

void
xpcc::atmega::pwm::Led8::setValue(uint8_t brightness)
{
	fading = false;
    if (brightness > maximum) brightness = maximum;
	
	xpcc::atomic::Lock lock;
    *compareRegister = ::xpcc::pwm::PwmValues8[brightness];
}

float
xpcc::atmega::pwm::Led8::getValue()
{
    return static_cast<float>(value)/maximum;
}

bool
xpcc::atmega::pwm::Led8::isFading()
{
    return fading;
}

void
xpcc::atmega::pwm::Led8::on(uint16_t fadeTime)
{
	if (fadeTime) {
		fadeTo(fadeTime, maximum);
	}
	else {
		fading = false;
		
		*compareRegister = ::xpcc::pwm::PwmValues8[maximum];
	}
}

void
xpcc::atmega::pwm::Led8::off(uint16_t fadeTime)
{
	if (fadeTime) {
		fadeTo(fadeTime, static_cast<uint8_t>(0));
	}
	else {
		fading = false;
		
		*compareRegister = ::xpcc::pwm::PwmValues8[0];
	}
}

void
xpcc::atmega::pwm::Led8::fadeTo(uint16_t time, float brightness)
{
	uint8_t fadeValue = brightness * maximum;
	
	fadeTo(time, static_cast<uint8_t>(fadeValue));
}

void
xpcc::atmega::pwm::Led8::fadeTo(uint16_t time, uint8_t brightness)
{
	if (brightness == value) return;
	
    startValue = value;
    endValue = brightness;
	
    fadeTime = static_cast<uint16_t>(::xpcc::math::mul(time, 10u)/abs(startValue - endValue));
	if (!fadeTime) fadeTime = 1;
	
	fading = true;
}

bool
xpcc::atmega::pwm::Led8::run()
{
    PT_BEGIN();
    while (true)
    {
        timer.restart(fadeTime);
        
        if (fading)
		{
            if (startValue < endValue)
			{
                // Fading up
                if (++value >= endValue) fading = false;
				
                *compareRegister = ::xpcc::pwm::PwmValues8[value];
            }
			else if (startValue > endValue)
			{
                // Fading down
                if (--value <= endValue) fading = false;
				
                *compareRegister = ::xpcc::pwm::PwmValues8[value];
            }
			else fading = false;
        }
        PT_WAIT_UNTIL(timer.isExpired());
    }
    PT_END();
}
// ----------------------------------------------------------------------------
