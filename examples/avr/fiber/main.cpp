/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2013, 2015-2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform.hpp>
#include <modm/architecture/interface/interrupt.hpp>
#include <modm/processing.hpp>

using namespace modm::platform;
using namespace std::chrono_literals;

using LedGreen = GpioOutputB0;
using LedRed = GpioOutputB1;

modm::Fiber fiber_green([]
{
	LedGreen::setOutput();
	LedGreen::set();

	while (true)
	{
		LedGreen::set();
		modm::this_fiber::sleep_for(100ms);

		LedGreen::reset();
		modm::this_fiber::sleep_for(600ms);
	}
});

modm::Fiber fiber_red([]
{
	LedRed::setOutput();
	LedRed::set();

	while (true)
	{
		LedRed::set();
		modm::this_fiber::sleep_for(200ms);

		LedRed::reset();
		modm::this_fiber::sleep_for(300ms);

		LedRed::set();
		modm::this_fiber::sleep_for(200ms);

		LedRed::reset();
		modm::this_fiber::sleep_for(1s);
	}
});

int
main()
{
	SystemClock::enable();
	enableInterrupts();

	modm::fiber::Scheduler::run();
}
