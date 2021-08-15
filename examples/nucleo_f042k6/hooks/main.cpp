/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 * Copyright (c) 2019, Benjamin Weps
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	uint32_t counter(0);

	while (true)
	{
		modm::delayMilliseconds(Button::read() ? 100 : 500);

		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
	}

	return 0;
}

//This hook will be triggered, whenever an interrupt occurs on the USART
MODM_ISR_ENTRY_HOOK(USART2)
{
	//Toggle the LED whenever we received a byte
	if (modm::platform::UsartHal2::isReceiveRegisterNotEmpty())
	{
		LedD13::toggle();
	}
}