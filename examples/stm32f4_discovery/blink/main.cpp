/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012, 2014, Sascha Schade
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2013, 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	LedOrange::set();
	LedRed::set();

	while (1)
	{
		LedBlue::toggle();
		LedGreen::toggle();
		LedOrange::toggle();
		LedRed::toggle();
		xpcc::delayMilliseconds(Button::read() ? 250 : 500);
	}

	return 0;
}
