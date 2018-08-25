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

typedef GpioInputE11 Irq;


/* When you choose a different pin you must choose the corresponding
 * interrupt handler: (x in A, B, C, D, E, F, G, H, I)
 * Px0:          EXTI0
 * Px1:          EXTI1
 * Px2:          EXTI2
 * Px3:          EXTI3
 * Px4:          EXTI4
 * Px5  to Px9:  EXTI9_5
 * Px10 to Px15: EXTI15_10
 */
MODM_ISR(EXTI0)
{
	Button::acknowledgeExternalInterruptFlag();
	LedBlue::set();
	modm::delayMicroseconds(1000);
	LedBlue::reset();
}


MODM_ISR(EXTI15_10)
{
	Irq::acknowledgeExternalInterruptFlag();
	LedOrange::set();
	modm::delayMicroseconds(1000);
	LedOrange::reset();
}

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
	Button::setInputTrigger(Button::InputTrigger::RisingEdge);
	Button::enableExternalInterrupt();
	Button::enableExternalInterruptVector(14);

	// pull pin E11 low to see the orange led light up
	Irq::setInput(Gpio::InputType::PullUp);
	Irq::setInputTrigger(Irq::InputTrigger::BothEdges);
	Irq::enableExternalInterrupt();
	Irq::enableExternalInterruptVector(14);

	while (1)
	{
		LedRed::toggle();
		LedGreen::toggle();
		modm::delayMilliseconds(500);
	}

	return 0;
}
