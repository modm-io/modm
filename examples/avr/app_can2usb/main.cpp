// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>

using namespace xpcc::at90;

typedef GpioOutputD7 Led;
// create a wrapper for a output pin
typedef xpcc::GpioInverted< GpioOutputB1 > LedInverted;

MAIN_FUNCTION
{
	Led::setOutput();
	Led::set();

	while (1)
	{
		xpcc::delayMilliseconds(1000);
		Led::toggle();
	}
}
