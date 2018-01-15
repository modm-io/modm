/*
 * Copyright (c) 2014-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef LEDS_HPP
#define LEDS_HPP

#include <modm/ui/led.hpp>

// create the leds with these lambda callbacks
modm::ui::Led orange([](uint8_t brightness)
{
	Timer4::setCompareValue(2, modm::ui::table22_16_256[brightness]);
});

modm::ui::Led red([](uint8_t brightness)
{
	Timer4::setCompareValue(3, modm::ui::table22_16_256[brightness]);
});

modm::ui::Led green([](uint8_t brightness)
{
	Timer4::setCompareValue(1, modm::ui::table22_16_256[brightness]);
});

modm::ui::Led blue([](uint8_t brightness)
{
	Timer4::setCompareValue(4, modm::ui::table22_16_256[brightness]);
});

#endif // LEDS_HPP
