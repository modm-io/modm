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

#ifndef XPCC__PWM_RGB_LED_HPP
#	error	"Don't include this file directly, use 'rgb.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

template< typename Red, typename Green, typename Blue >
xpcc::led::Rgb<Red, Green, Blue>::Rgb()
{
}

template< typename Red, typename Green, typename Blue >
void
xpcc::led::Rgb<Red, Green, Blue>::setBrightness(uint16_t redValue, uint16_t greenValue, uint16_t blueValue)
{
    red.setValue(redValue);
    green.setValue(greenValue);
	blue.setValue(blueValue);
}

template< typename Red, typename Green, typename Blue >
uint16_t
xpcc::led::Rgb<Red, Green, Blue>::getBrightness(Color color)
{
    switch (color) {
        case RED:
            return red.getValue();
        case GREEN:
            return green.getValue();
        case BLUE:
            return blue.getValue();
        default:
            return 0;
    }
}

template< typename Red, typename Green, typename Blue >
void
xpcc::led::Rgb<Red, Green, Blue>::fadeTo(uint16_t time, uint16_t redValue, uint16_t greenValue, uint16_t blueValue)
{
    red.fadeTo(time, redValue);
    green.fadeTo(time, greenValue);
    blue.fadeTo(time, blueValue);
}

template< typename Red, typename Green, typename Blue >
bool
xpcc::led::Rgb<Red, Green, Blue>::isFading()
{
    return (red.isFading() ||
            green.isFading() ||
            blue.isFading());
}

template< typename Red, typename Green, typename Blue >
void
xpcc::led::Rgb<Red, Green, Blue>::run()
{
    red.run();
    green.run();
    blue.run();
}
