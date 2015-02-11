// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>
#include <xpcc/math/utils/misc.hpp>
#include <cmath>

using namespace xpcc::atmega;

typedef GpioOutputB0 Led;
// create a wrapper for a output pin
typedef xpcc::GpioInverted< GpioOutputB1 > LedInverted;

int32_t clock(0);
volatile bool isPositive(false);

MAIN_FUNCTION
{
//	Led::setOutput();
//	Led::set();
//
//	LedInverted::setOutput(xpcc::Gpio::Low);

//	isPositive = clock > 0;
	isPositive = xpcc::isPositive(*((float*)&clock));
	(void) isPositive;

	while (1)
	{
	}
}
