// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>

using namespace xpcc::atmega;

// create a wrapper for a output pin

typedef GpioOutputB0 Led;
typedef GpioOutputB1 Led2;

typedef xpcc::GpioInverted< Led2 > LedInverted;

int
main(void)
{
	Led::setOutput();
	Led::set();

	LedInverted::reset();

	while (1)
	{
	}
}
