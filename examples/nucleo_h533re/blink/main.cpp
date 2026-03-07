/*
 * Copyright (c) 2016-2017, Niklas Hauser
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

using namespace Board;

int
main()
{
	Board::initialize();
	Leds::setOutput();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	for (const auto [traits, start, end, size] : modm::platform::HeapTable())
	{
		MODM_LOG_INFO.printf("Memory section %#x @[0x%p,0x%p](%u)\n",
							 traits.value, start, end, size);
	}

	uint32_t counter(0);
	modm::ShortTimeout tmr;

	while (true)
	{
		Leds::write(1 << (counter % (Leds::width+1) ));
		// modm::delay(Button::read() ? 100ms : 500ms);
		tmr.restart(Button::read() ? 100ms : 500ms);
		while(not tmr.execute()) ;

		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
	}

	return 0;
}
