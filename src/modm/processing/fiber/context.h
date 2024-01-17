/*
 * Copyright (c) 2020, Erik Henriksson
 * Copyright (c) 2021, 2023, Niklas Hauser
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
#include <stdbool.h>
#include <modm/architecture/utils.hpp>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @ingroup modm_processing_fiber
 * @defgroup modm_processing_fiber_context Fiber Context Functions
 *
 * These functions implement the underlying architecture specific operations to
 * initialize, start, yield, and end fibers. These can be used to implement an
 * alternative scheduling strategy or use fibers outside of modm.
 * @{
 */

/// The fiber context stores information about the stack.
struct modm_context_t
{
	uintptr_t *sp;		///< Stored stack pointer, only valid when *not* running!
	uintptr_t *bottom;	///< Bottom of stack
	uintptr_t *top;		///< Top of stack, one word beyond stack!
};

/**
 * Initializes the context with the function pointer and argument stored at the
 * top. The `ctx->sp` is set to two `uintptr_t` below the top.
 * Must be called only once to initialize the stack.
 *
 * @param fn function pointer to a `void(*)(uintptr_t)` function.
 */
void
modm_context_init(modm_context_t *ctx,
				  uintptr_t *bottom, uintptr_t *top,
				  uintptr_t fn, uintptr_t arg);

/**
 * Resets the context stack pointer to the correct offset in preparation for
 * jumping into the fiber. Must be called after initialization, but every time
 * the function is started from the beginning.
 */
void
modm_context_reset(modm_context_t *ctx);

/**
 * Switches control from the main context to the fiber context. This initializes
 * the hardware and then jumps from the caller context into the `to` fiber.
 */
void
modm_context_start(modm_context_t *to);

/**
 * Pushes the context onto the `from->sp` and pops the context from the `to->sp`
 * to jump from one fiber to the next.
 */
void
modm_context_jump(modm_context_t *from, modm_context_t *to) asm("modm_context_jump");

/**
 * Switches control from the fiber context back to the main context.
 * Control flow then continues in the main context by returning from the
 * `modm_context_start()` function.
 */
void modm_noreturn
modm_context_end();

/**
 * Zeros the register file and watermarks the rest of the stack.
 * You may call this function before or after `modm_context_reset()`, however,
 * *NOT* while the fiber is running!
 */
void
modm_context_watermark(modm_context_t *ctx);

/**
 * Returns the stack usage by searching from the bottom of the stack for the
 * watermark level. You may call this function at any point after calling
 * `modm_context_watermark()`.
 */
size_t
modm_context_stack_usage(const modm_context_t *ctx);

/// A cheap way to check if the last word of the stack was written.
bool
modm_context_stack_overflow(const modm_context_t *ctx);
/// @}

#ifdef __cplusplus
}
#endif
