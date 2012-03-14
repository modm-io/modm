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
#	error "Don't include this file directly, use 'tlc594x_led.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename PwmController, uint16_t CHANNEL, const uint16_t* PwmTable,  uint16_t const PwmTableSize >
xpcc::led::TLC594XLed<PwmController, CHANNEL, PwmTable, PwmTableSize>::TLC594XLed()
:	currentValue(0), deltaValue(0), startValue(0), endValue(0), fadeTime(0), timer(1), table(PwmTable)
{
	PwmController::setChannel(CHANNEL, table[0]);
}

template < typename PwmController, uint16_t CHANNEL, const uint16_t* PwmTable,  uint16_t const PwmTableSize >
void
xpcc::led::TLC594XLed<PwmController, CHANNEL, PwmTable, PwmTableSize>::setBrightness(uint16_t brightness)
{
	fadeTime = 0;
	if (brightness > PwmTableSize-1) brightness = PwmTableSize-1;
	currentValue = brightness;
	PwmController::setChannel(CHANNEL, table[currentValue]);
}

template < typename PwmController, uint16_t CHANNEL, const uint16_t* PwmTable,  uint16_t const PwmTableSize >
uint16_t
xpcc::led::TLC594XLed<PwmController, CHANNEL, PwmTable, PwmTableSize>::getBrightness()
{
	return currentValue;
}

template < typename PwmController, uint16_t CHANNEL, const uint16_t* PwmTable,  uint16_t const PwmTableSize >
bool
xpcc::led::TLC594XLed<PwmController, CHANNEL, PwmTable, PwmTableSize>::isFading()
{
	return static_cast<bool>(fadeTime);
}

template < typename PwmController, uint16_t CHANNEL, const uint16_t* PwmTable,  uint16_t const PwmTableSize >
void
xpcc::led::TLC594XLed<PwmController, CHANNEL, PwmTable, PwmTableSize>::fadeTo(uint16_t time, uint16_t brightness)
{
	if (brightness == currentValue) return;
	if (brightness > PwmTableSize-1) brightness = PwmTableSize-1;
	if (!time) {
		currentValue = brightness;
		PwmController::setChannel(CHANNEL, table[currentValue]);
	}
	else {
		startValue = currentValue;
		endValue = brightness;
		deltaValue = (endValue - startValue) / static_cast<float>(time);
	}
	fadeTime = time;
}

template < typename PwmController, uint16_t CHANNEL, const uint16_t* PwmTable,  uint16_t const PwmTableSize >
void
xpcc::led::TLC594XLed<PwmController, CHANNEL, PwmTable, PwmTableSize>::on(uint16_t time)
{
	fadeTo(time, PwmTableSize-1);
}

template < typename PwmController, uint16_t CHANNEL, const uint16_t* PwmTable,  uint16_t const PwmTableSize >
void
xpcc::led::TLC594XLed<PwmController, CHANNEL, PwmTable, PwmTableSize>::off(uint16_t time)
{
	fadeTo(time, 0);
}

template < typename PwmController, uint16_t CHANNEL, const uint16_t* PwmTable,  uint16_t const PwmTableSize >
void
xpcc::led::TLC594XLed<PwmController, CHANNEL, PwmTable, PwmTableSize>::run()
{
	if (timer.isExpired() && fadeTime)
	{
		startValue += deltaValue;
		currentValue = static_cast<uint16_t>(startValue);
		if (!--fadeTime) currentValue = endValue;
		
		PwmController::setChannel(CHANNEL, table[currentValue]);
	}
}




