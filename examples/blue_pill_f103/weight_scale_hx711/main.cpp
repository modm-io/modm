/*
 * Copyright (c) 2020, Sascha Schade
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
#include <modm/driver/adc/hx711.hpp>

using namespace Board;


struct hx711_config : public modm::hx711::Config
{
	using Sck  = GpioOutputA9;
	using Data = GpioInputA10;
	// static const modm::hx711::InputChannelAndGain mode = modm::hx711::InputChannelAndGain::ChA_64;
};
using Hx711 = modm::Hx711< hx711_config >;
Hx711 hx711;

modm::Fiber fiber_hx711([]
{
	while (true)
	{
		const int32_t result = hx711.singleConversion();
		MODM_LOG_DEBUG.printf("%" PRIi32 "\n", result);
	}
});

modm::Fiber fiber_blink([]
{
	while (true)
	{
		modm::this_fiber::sleep_for(1s);
		LedGreen::toggle();
	}
});

/*
 * Blinks the green user LED with 1 Hz while measuring.
 *
 * Make the following connections to HX711:
 *   Data PA10
 *   Clock PA9
 */
int
main()
{
	Board::initialize();

	// Use the logging streams to print some messages.
	MODM_LOG_DEBUG << "HX711 demo" << modm::endl;

	hx711_config::Sck::setOutput();
	hx711_config::Data::setInput();

	modm::fiber::Scheduler::run();

	return 0;
}
