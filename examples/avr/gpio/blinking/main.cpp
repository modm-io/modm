/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2013-2014, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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

