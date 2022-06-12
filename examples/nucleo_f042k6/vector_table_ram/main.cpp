/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

// Blink LED with timer 14 interrupt. A custom handler is configured at runtime
// in the vector table located in SRAM.

// If the LED is blinking with a period of 1 second, the vector table has been
// successfully relocated to ram.
// Set the option "modm:platform:core:vector_table_location" in project.xml
// to place the vector table in ram on F0 devices without vector table relocation
// support in the Cortex M0 core.

static void tim14Handler()
{
	Timer14::acknowledgeInterruptFlags(Timer14::InterruptFlag::Update);
	LedD13::toggle();
}

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	// Set custom handler, only works if vector table is in RAM
	NVIC_SetVector(TIM14_IRQn, reinterpret_cast<uintptr_t>(&tim14Handler));

	Timer14::enable();
	Timer14::setMode(Timer14::Mode::UpCounter);
	Timer14::setPeriod<Board::SystemClock>(500ms);
	Timer14::applyAndReset();
	Timer14::start();
	Timer14::enableInterrupt(Timer14::Interrupt::Update);
	Timer14::enableInterruptVector(true, 5);

	uint32_t counter{0};
	while (true)
	{
		modm::delay(100ms);
		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
	}

	return 0;
}
