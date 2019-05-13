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

using namespace Board;

MODM_ISR(EXTI0)
{ MODM_LOG_DEBUG << "EXTI0 called!" << modm::endl; }
MODM_ISR(EXTI1)
{ MODM_LOG_DEBUG << "EXTI1 called!" << modm::endl; }
MODM_ISR(EXTI2)
{ MODM_LOG_DEBUG << "EXTI2 called!" << modm::endl; }
MODM_ISR(EXTI3)
{ MODM_LOG_DEBUG << "EXTI3 called!" << modm::endl; }

// But we forgot about EXTI4
// MODM_ISR(EXTI4)
// { MODM_LOG_DEBUG << "EXTI4 called!" << modm::endl; }

static modm::Abandonment
core_assertion_handler(const char * module,
					   const char * /*location*/,
					   const char * /*failure*/,
					   uintptr_t context)
{
	if (!memcmp(module, "core\0nvic\0undefined", 19)) {
		MODM_LOG_ERROR.printf("Ignoring undefined IRQ handler %d!\n", int8_t(context));
		return modm::Abandonment::Ignore;
	}
	return modm::Abandonment::DontCare;
}
// Uncomment to ignore the assertion
// MODM_ASSERTION_HANDLER(core_assertion_handler);

// Uncomment to overwrite the undefined handler
// extern "C" void modm_undefined_handler(int32_t) {}

int main()
{
	Board::initialize();
	// Enable the Interrupt handlers
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_EnableIRQ(EXTI4_IRQn);
	// Give them the highest priority
	NVIC_SetPriority(EXTI0_IRQn, 0);
	NVIC_SetPriority(EXTI1_IRQn, 0);
	NVIC_SetPriority(EXTI2_IRQn, 0);
	NVIC_SetPriority(EXTI3_IRQn, 0);
	NVIC_SetPriority(EXTI4_IRQn, 0);

	MODM_LOG_INFO << "Push the Button to trigger EXTI interrupts!" << modm::endl;
	int ii{0};

	while (1)
	{
		if(Button::read())
		{
			NVIC_SetPendingIRQ(IRQn_Type(int(EXTI0_IRQn) + ii));
			ii = (ii + 1) % 5;
			// wait one second for debounce
			modm::delayMilliseconds(500);
		}
	}

	return 0;
}
