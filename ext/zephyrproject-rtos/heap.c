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

#include <modm/architecture/interface/assert.h>

// Corruption found by the heap hardening checks
#define k_panic() modm_assert(0, "heap.corrupt", "Zephyr heap corruption detected!")

// The internal checks of the Zephyr heap only run in debug builds
#ifdef MODM_DEBUG_BUILD
#	define __ASSERT(x, ...) modm_assert_continue_fail_debug(x, "heap.zephyr", "Zephyr heap check failed!")
#endif

#include "heap.c.inc"
