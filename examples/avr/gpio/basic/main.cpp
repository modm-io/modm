/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2013-2016, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>

using namespace xpcc::attiny;

typedef GpioOutputB0 Led;
// create a wrapper for a output pin
typedef xpcc::GpioInverted< GpioOutputB1 > LedInverted;

int
main()
{
	Led::setOutput();
	Led::set();

	LedInverted::setOutput(xpcc::Gpio::Low);

	while (1)
	{
	}
}
