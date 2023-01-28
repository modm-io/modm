/*
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	uint32_t counter = 0;

	while (true)
	{
		Leds::write(counter % (1 << 3));
		modm::delay(Button::read() ? 250ms : 500ms);
		MODM_LOG_INFO << "loop: " << counter << modm::endl;
		counter++;
	}

	return 0;
}
