/*
 * Copyright (c) 2022, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

/**
 * Pico SDK compatibility for tinyusb
 */
#include "hardware/regs/intctrl.h"

static inline void
irq_set_enabled(int irqn, bool enable)
{
	if (enable) NVIC_EnableIRQ(irqn);
	else NVIC_DisableIRQ(irqn);
}

static inline void
irq_set_exclusive_handler(int irqn, void (*handler)(void))
{
	(void) irqn;
	(void) handler;
	// do nothing, irq implemented at modm
}

static inline void
irq_add_shared_handler(unsigned int irqn, void (*handler)(void), uint8_t order_priority)
{
	(void) irqn;
	(void) handler;
	(void) order_priority;
}

#define PICO_SHARED_IRQ_HANDLER_HIGHEST_ORDER_PRIORITY 0xff
