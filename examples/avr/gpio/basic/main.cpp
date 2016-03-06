// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
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
