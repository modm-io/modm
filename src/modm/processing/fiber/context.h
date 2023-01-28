/*
 * Copyright (c) 2020, Erik Henriksson
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <modm/architecture/utils.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

/// @ingroup    modm_processing_fiber
/// @{
struct modm_context_t
{
	uintptr_t sp;
	size_t stack_size;
};

/// Prepares the stack to jump into function with arg and call end on return
modm_context_t
modm_context_init(uintptr_t stack, uintptr_t arg, uintptr_t fn, uintptr_t end);

/// Switches control from the main context to the user context.
void
modm_context_start(uintptr_t sp);

/// Jumps from the "from" user context to the "to" user context.
void
modm_context_jump(uintptr_t* from_sp, uintptr_t to_sp);

/// Switches control back to the main context from the user context.
void modm_noreturn
modm_context_end();
/// @}

#ifdef __cplusplus
}
#endif
