/*
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012-2014, Sascha Schade
 * Copyright (c) 2013-2014, Kevin Läufer
 * Copyright (c) 2013, 2015-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/**
 * This example is to demonstrate the External Interrupt (EXTI)
 *
 * The red/green LEDs toggles by busy waiting.
 * When the button is pressed the blue LED flashes
 * for a very short moment in the interrupt handler.
 *
 */

#include <modm/board.hpp>
#include <modm/architecture/interface/interrupt.hpp>

using namespace Board;
using Irq = GpioInputE11;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// will be used to indicate external interrupts
	LedOrange::setOutput(modm::Gpio::Low);
	LedBlue::setOutput(modm::Gpio::Low);

	// will be toggled to indicate that the program is still running
	LedGreen::setOutput(modm::Gpio::Low);
	LedRed::setOutput(modm::Gpio::High);

	// push the button to see the blue led light up
	Button::setInput(Gpio::InputType::Floating);
	Exti::connect<Button>(Exti::Trigger::RisingEdge, [](uint8_t)
	{
		LedBlue::set();
		modm::delay(1ms);
		LedBlue::reset();
	});

	// pull pin E11 low to see the orange led light up
	Irq::setInput(Gpio::InputType::PullUp);
	Exti::connect<Irq>(Exti::Trigger::BothEdges, [](uint8_t)
	{
		LedOrange::set();
		modm::delay(1ms);
		LedOrange::reset();
	});

	while (true)
	{
		LedRed::toggle();
		LedGreen::toggle();
		modm::delay(500ms);
	}

	return 0;
}
