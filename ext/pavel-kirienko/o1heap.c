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

// The internal checks of o1heap only run in debug builds
#ifdef MODM_DEBUG_BUILD
#	define O1HEAP_ASSERT(x) modm_assert_continue_fail_debug(x, "o1heap", "o1heap internal check failed!")
#else
#	define O1HEAP_ASSERT(x) ((void)0)
#endif

#include "o1heap.c.inc"
