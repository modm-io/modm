/*
 * Copyright (c) 2012, Fabian Greif
 * Copyright (c) 2013-2015, Kevin Laeufer
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/architecture.hpp>

// ----------------------------------------------------------------------------
using namespace xpcc::lpc;

typedef GpioOutput0_7 Led;

// ----------------------------------------------------------------------------
int 
main()
{
	// New Static Clock Setup ( => Static)
	typedef Pll<ExternalCrystal<MHz12>, MHz48> clockSource;
	// typedef ExternalOscillator<MHz12> clockSource;
	SystemClock<clockSource>::enable();

	ClockControl::enableClockOutput(ClockControl::ClockOutputSource::SystemClock, 100);
	Gpio0_1::selectFunction(Gpio0_1::Function::ClockOutput);	// TODO: replace with connect

	Led::setOutput();

	while (1)
	{
		Led::toggle();
		xpcc::delayMilliseconds(100);
	}
}
