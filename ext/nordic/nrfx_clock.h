/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// Absolute minimum of the nrfx_clock.h header, only to fulfill what TinyUSB uses.

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"

typedef uint32_t nrf_clock_hfclk_t;
typedef uint32_t nrf_clock_event_t;
typedef uint32_t nrf_clock_task_t;

#ifndef NRF_CLOCK_HFCLK_HIGH_ACCURACY
#define NRF_CLOCK_HFCLK_HIGH_ACCURACY 1u
#endif

#ifndef NRF_CLOCK_EVENT_HFCLKSTARTED
#define NRF_CLOCK_EVENT_HFCLKSTARTED 0u
#endif

#ifndef NRF_CLOCK_TASK_HFCLKSTART
#define NRF_CLOCK_TASK_HFCLKSTART 0u
#endif

#ifndef NRF_CLOCK_TASK_HFCLKSTOP
#define NRF_CLOCK_TASK_HFCLKSTOP 1u
#endif

static inline bool
nrf_clock_hf_is_running(NRF_CLOCK_Type const * p_reg, nrf_clock_hfclk_t source)
{
	(void)source;
#if defined(CLOCK_HFCLKSTAT_STATE_Msk)
	return (p_reg->HFCLKSTAT & CLOCK_HFCLKSTAT_STATE_Msk) != 0u;
#elif defined(CLOCK_HFCLKSTAT_HFCLKRUN_Msk)
	return (p_reg->HFCLKSTAT & CLOCK_HFCLKSTAT_HFCLKRUN_Msk) != 0u;
#else
	return p_reg->HFCLKRUN != 0u;
#endif
}

static inline void
nrf_clock_event_clear(NRF_CLOCK_Type * p_reg, nrf_clock_event_t event)
{
	(void)event;
	p_reg->EVENTS_HFCLKSTARTED = 0u;
}

static inline void
nrf_clock_task_trigger(NRF_CLOCK_Type * p_reg, nrf_clock_task_t task)
{
	if (task == NRF_CLOCK_TASK_HFCLKSTOP) {
		p_reg->TASKS_HFCLKSTOP = 1u;
	}
	else {
		p_reg->TASKS_HFCLKSTART = 1u;
	}
}
