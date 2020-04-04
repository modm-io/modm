/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2013-2014, 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform.hpp>

using namespace modm::platform;
using namespace std::chrono_literals;

typedef GpioOutputB0 Led;

int
main()
{
	Led::setOutput();

	while (true)
	{
		Led::toggle();

		modm::delay(1s);
	}
}

