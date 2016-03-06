// coding: utf-8
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------
#include <xpcc/architecture.hpp>

using namespace xpcc::atmega;

typedef GpioOutputB0 Led;

int 
main()
{
	Led::setOutput();

	while(1)
	{
		Led::toggle();

		xpcc::delayMilliseconds(1000);
	}
}

