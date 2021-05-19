/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011-2014, 2017-2018, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include <modm/ui/color.hpp>
#include "led.hpp"

namespace modm::ui
{

/**
 * Group three LEDs into a RGB controller
 *
 * @todo add RGB specific functions to this class.
 *
 * @author	Niklas Hauser
 * @ingroup modm_ui_led
 */
class RgbLed
{
	Led& red;
	Led& green;
	Led& blue;

	::modm::color::Rgb absolute;

public:
	RgbLed(Led& red, Led& green, Led& blue):
		red(red), green(green), blue(blue) {}

	inline void
	setColor(::modm::color::Rgb color)
	{
		absolute = color;

		red.setBrightness(color.red);
		green.setBrightness(color.green);
		blue.setBrightness(color.blue);
	}

	inline ::modm::color::Rgb
	getColor()
	{
		return ::modm::color::Rgb(
			red.getBrightness(), green.getBrightness(), blue.getBrightness());
	}

	inline bool
	isFading()
	{
		return (red.isFading() || green.isFading() || blue.isFading());
	}

	inline void
	fadeTo(::modm::color::Rgb color, uint16_t time)
	{
		absolute = color;

		red.fadeTo(absolute.red, time);
		green.fadeTo(absolute.green, time);
		blue.fadeTo(absolute.blue, time);
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

}  // namespace modm::ui
