/*
 * Copyright (c) 2024, Elias H.
 * Copyright (c) 2024, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/radio/dw3110/dw3110_phy.hpp>

using namespace Board;

using MySpiMaster = modm::platform::SpiMaster1;
using MyDw3110 = modm::Dw3110Phy<MySpiMaster, GpioB6>;

MyDw3110 myDw3110;

int
main()
{
	Board::initialize();
	LedD13::setOutput();
	GpioB6::setOutput();

	MySpiMaster::initialize<Board::SystemClock, 21_MHz>();
	MySpiMaster::connect<GpioA6::Miso, GpioA7::Mosi, GpioA5::Sck>();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	RF_CALL_BLOCKING(myDw3110.initialize());

	uint32_t counter(0);

	while (true)
	{
		LedD13::toggle();
		modm::delay(Button::read() ? 100ms : 500ms);

		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
	}

	return 0;
}
