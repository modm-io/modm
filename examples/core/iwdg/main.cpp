/*
 * Copyright (c) 2023, Zühlke Engineering (Austria) GmbH
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/platform.hpp>

using namespace Board;
using namespace std::chrono_literals;

/**
 * If the button is pressed for more than 4 seconds, the MCU will be reset by the Watchdog.
 * This can be observed by the faster blinking LED after startup.
 */

int
main()
{
	Board::initialize();
	LedD13::setOutput();
	// set the watchdog timeout to 4 seconds
	Iwdg::initialize<SystemClock, 4s>();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG << "debug" << modm::endl;
	MODM_LOG_INFO << "info" << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR << "error" << modm::endl;

	uint32_t counter(0);

	while (counter < 10)
	{
		LedD13::toggle();
		modm::delay(100ms);
		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
	}

	Iwdg::enable();

	while (1)
	{
		LedD13::toggle();
		modm::delay(500ms);
		if (!Button::read()) { Iwdg::trigger(); }
		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
	}

	return 0;
}
