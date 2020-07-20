/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "extint.hpp"

void isr();

namespace modm
{
namespace platform
{

void
ExternalInterrupt::initialize(ClockGenerator clockGen, int priority) {
	NVIC_DisableIRQ(EIC_IRQn);
	NVIC_ClearPendingIRQ(EIC_IRQn);
	NVIC_SetPriority(EIC_IRQn, priority);
	NVIC_EnableIRQ(EIC_IRQn);

	GenericClockController::connect<ClockPeripheral::Eic>(clockGen);

	// Enable EIC
	EIC->CTRL.bit.ENABLE = 1;
	int waitCycles = 2048;
	while (EIC->STATUS.bit.SYNCBUSY == 1 && --waitCycles);
}

std::array<std::function<void()>, 16> ExternalInterrupt::handlers_ = {};

// FIXME: Figure out if it is worth using function pointers here instead
//        to get rid of std::function overhead.
MODM_ISR(EIC) {
	uint32_t int_flag = EIC->INTFLAG.reg;
	uint32_t flags = int_flag << 16;
	auto handler = ExternalInterrupt::handlers_.end() - 1;
	do {
		uint8_t leading_zeros = __builtin_clz(flags);
		handler -= leading_zeros;
		flags <<= leading_zeros;
		if (flags & 1u << 31 && *handler) {
			(*handler)();
			flags &= ~(1u << 31);
		}
	} while (flags);
	EIC->INTFLAG.reg |= int_flag;
}


}	// namespace platform
}	// namespace modm
