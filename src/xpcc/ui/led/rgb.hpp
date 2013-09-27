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
	::xpcc::color::Rgb relative;

public:
	RgbLed(Led& red, Led& green, Led& blue)
	:	red(red), green(green), blue(blue), absolute(), relative()
	{
	}

	inline void
	setColor(::xpcc::color::Rgb color)
	{
		absolute = color;
		relative = absolute.getRelativeColors<uint16_t, 255>();

		red.setBrightness(relative.red);
		green.setBrightness(relative.green);
		blue.setBrightness(relative.blue);
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
		relative = absolute.getRelativeColors<uint16_t, 255>();

		red.fadeTo(time, relative.red);
		green.fadeTo(time, relative.green);
		blue.fadeTo(time, relative.blue);
	}

	inline void
	fadeTo(uint16_t time, ::xpcc::color::Hsv color)
	{
		color.toRgb(&absolute);
		fadeTo(time, absolute);
	}

	/// Must be called at least every ms
	inline void
	run()
	{
		red.run();
		green.run();
		blue.run();
	}
};

}

}

#endif	// XPCC__PWM_RGB_LED_HPP
