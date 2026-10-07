/*
 * Copyright (c) 2022, Nikolay Semenov
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

int
main()
{
	Board::initialize();

	using Led = Board::LedGreen;
	Led::setOutput();
	Led::set();

	// Powers on the LED on the low->high transition, and off on high->low.
	GpioInput0::setInput();
	IntHandler::connect<GpioInput0>(Gpio::InputTrigger::BothEdges,
									[](Gpio::InputTrigger_t triggers) {
										Led::set(!!(triggers & Gpio::InputTrigger::RisingEdge));
									});

	// Toggles LED each time gpio input is at the high level.
	GpioInput1::setInput(Gpio::InputType::PullDown);
	IntHandler::connect<GpioInput1>(Gpio::InputTrigger::HighLevel,
									[](Gpio::InputTrigger_t) { Led::toggle(); });

	while (true) {}

	return 0;
}
