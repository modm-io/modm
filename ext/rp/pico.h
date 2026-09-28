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

#include <modm/architecture/interface/assert.h>
#include <stdint.h>
#include <stddef.h>
/* need for static_assert at C compilation */
#include <assert.h>
#define TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX 0

#ifdef __cplusplus
extern "C" {
#endif
/* override expensive assert implementation */
#undef assert
#define assert(X) modm_assert((X), "pico", __FILE__ ":" MODM_STRINGIFY(__LINE__) " -> \"" #X "\"")

#ifndef hard_assert
#define hard_assert assert
#endif

typedef unsigned int uint;

void
panic(const char* /*fmt*/, ...);

uint8_t rp2040_chip_version(void);

static inline void
busy_wait_at_least_cycles(uint32_t minimum_cycles)
{
	asm volatile(
		"1: sub %0, #3\t\n"
		"bcs 1b\t\n"
		: "+l" (minimum_cycles) : : "cc", "memory"
	);
}

#ifdef __cplusplus
}
#endif
