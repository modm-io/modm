/*
 * Copyright (c) 2014-2016, Niklas Hauser
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

#include <xpcc/architecture/platform.hpp>
#include <xpcc/ui/led.hpp>

// create the leds with these lambda callbacks
xpcc::ui::Led orange([](uint8_t brightness)
{
	Timer4::setCompareValue(2, xpcc::ui::table22_16_256[brightness]);
});

xpcc::ui::Led red([](uint8_t brightness)
{
	Timer4::setCompareValue(3, xpcc::ui::table22_16_256[brightness]);
});

xpcc::ui::Led green([](uint8_t brightness)
{
	Timer4::setCompareValue(1, xpcc::ui::table22_16_256[brightness]);
});

xpcc::ui::Led blue([](uint8_t brightness)
{
	Timer4::setCompareValue(4, xpcc::ui::table22_16_256[brightness]);
});

#endif // LEDS_HPP
