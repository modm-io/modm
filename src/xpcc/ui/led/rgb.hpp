// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_UI_RGB_LED_HPP
#define XPCC_UI_RGB_LED_HPP

#include <stdint.h>
#include "led.hpp"
#include "../color.hpp"

namespace xpcc
{

namespace ui
{

/**
 * Group three LEDs into a RGB controller
 *
 * @todo add RGB specific functions to this class.
 *
 * @author	Niklas Hauser
 * @ingroup led
 */
class RgbLed
{
	Led& red;
	Led& green;
	Led& blue;

	::xpcc::color::Rgb absolute;

public:
	RgbLed(Led& red, Led& green, Led& blue)
	:	red(red), green(green), blue(blue), absolute()
	{
	}

	inline void
	setColor(::xpcc::color::Rgb color)
	{
		absolute = color;

		red.setBrightness(absolute.red);
		green.setBrightness(absolute.green);
		blue.setBrightness(absolute.blue);
	}

	inline void
	setColor(::xpcc::color::Hsv color)
	{
		color.toRgb(&absolute);
		setColor(absolute);
	}

	inline ::xpcc::color::Rgb
	getColor()
	{
		return ::xpcc::color::Rgb(
			red.getBrightness(),
			green.getBrightness(),
			blue.getBrightness()
		);
	}

	inline bool
	isFading()
	{
		return (red.isFading() ||
				green.isFading() ||
				blue.isFading());
	}

	inline void
	fadeTo(uint16_t time, ::xpcc::color::Rgb color)
	{
		absolute = color;

		red.fadeTo(time, absolute.red);
		green.fadeTo(time, absolute.green);
		blue.fadeTo(time, absolute.blue);
	}

	inline void
	fadeTo(uint16_t time, ::xpcc::color::Hsv color)
	{
		color.toRgb(&absolute);
		fadeTo(time, absolute);
	}

	/// should be called every 1ms or more.
	inline void
	update()
	{
		red.update();
		green.update();
		blue.update();
	}
};

}

}

#endif	// XPCC__PWM_RGB_LED_HPP
