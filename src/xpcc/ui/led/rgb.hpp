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

public:
	enum class
	Color
	{
		Red,
		Green,
		Blue
	};

public:
	RgbLed(Led& red, Led& green, Led& blue)
	:	red(red), green(green), blue(blue)
	{
	}

	inline void
	setBrightness(uint16_t redValue, uint16_t greenValue, uint16_t blueValue)
	{
		red.setBrightness(redValue);
		green.setBrightness(greenValue);
		blue.setBrightness(blueValue);
	}

	inline uint16_t
	getBrightness(Color color)
	{
		switch (color) {
			case Color::Red:
				return red.getBrightness();
			case Color::Green:
				return green.getBrightness();
			case Color::Blue:
				return blue.getBrightness();
			default:
				return 0;
		}
	}

	inline bool
	isFading()
	{
		return (red.isFading() ||
				green.isFading() ||
				blue.isFading());
	}

	inline void
	fadeTo(uint16_t time, uint16_t redValue, uint16_t greenValue, uint16_t blueValue)
	{
		red.fadeTo(time, redValue);
		green.fadeTo(time, greenValue);
		blue.fadeTo(time, blueValue);
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
