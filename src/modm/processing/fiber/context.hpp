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

#pragma once

#include <modm/architecture/utils.hpp>

#include <cstddef>

typedef void* modm_stack_t;

struct modm_context {
  modm_stack_t* sp;
  std::size_t stack_size = 0;
};

extern modm_context main_context;

/* Prepares the stack to look like the "from" arg after modm_jumpcontext as run. */
modm_context
modm_makecontext(modm_stack_t* stack, std::size_t stack_size, void (*fn)(void), void (*end)(void));

/* Switches control from the main context to the user context. */
void
modm_startcontext(const modm_context &to);

/* Jumps from the "from" user context to the "to" user context. */
void
modm_jumpcontext(modm_context* from, const modm_context &to);

/* Switches control back to the main context from the user context. */
void
modm_endcontext();
