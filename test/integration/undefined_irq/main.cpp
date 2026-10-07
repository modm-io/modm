/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/architecture/interface/assert.hpp>
#include <cstring>

#include "../integration_test.hpp"

/**
 * Tests that an interrupt without a handler fails the `nvic.undef` assertion
 * with the number of the interrupt, while the defined handlers are called.
 */

static volatile uint8_t called{0};

MODM_ISR(EXTI0) { called++; }
MODM_ISR(EXTI1) { called++; }
MODM_ISR(EXTI2) { called++; }
MODM_ISR(EXTI3) { called++; }

// But we forgot about EXTI4
// MODM_ISR(EXTI4) { called++; }

static volatile uint8_t undefined{0};
static volatile int8_t undefined_irq{-1};

static modm::Abandonment
core_assertion_handler(const modm::AssertionInfo &info)
{
	if (strcmp(info.name, "nvic.undef") == 0) {
		undefined++;
		undefined_irq = int8_t(info.context);
		return modm::Abandonment::Ignore;
	}
	return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER(core_assertion_handler);

int main()
{
	Board::initialize();

	for (int ii = 0; ii < 5; ii++)
	{
		const auto irq = IRQn_Type(int(EXTI0_IRQn) + ii);
		NVIC_SetPriority(irq, 0);
		NVIC_EnableIRQ(irq);
		NVIC_SetPendingIRQ(irq);
		modm::delay(1ms);
	}
	MODM_LOG_INFO << called << " handlers called, " << undefined << " undefined: IRQ "
				  << undefined_irq << modm::endl;

	return finishTest(called == 4 and undefined == 1 and undefined_irq == int8_t(EXTI4_IRQn));
}
