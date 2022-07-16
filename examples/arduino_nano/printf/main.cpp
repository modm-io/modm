/*
 * Copyright (c) 2022, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing.hpp>

using namespace modm::platform;

int
main()
{
	Board::initialize();
	LedD13::setOutput();
	modm::PeriodicTimer heartbeat(1s);

	// <option name="modm:io:with_long_long">yes</option>
	serialStream << 32ull << modm::endl;

	// <option name="modm:io:with_float">yes</option>
	serialStream << 32.0f << modm::endl;

	// <option name="modm:io:with_printf">yes</option>
	serialStream.printf("hello %lu %03.3f\n", 32ul, 32.23451);

	uint8_t counter{0};
	while (true)
	{
		if (heartbeat.execute())
		{
			serialStream << counter++ << modm::endl;
			Board::LedD13::toggle();
		}
	}
}
