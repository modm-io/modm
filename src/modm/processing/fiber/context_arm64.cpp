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

#include "context.h"
#include <modm/architecture/detect.hpp>

/* Stack layout (growing downwards):
 *
 * Permanent Storage:
 * Fiber Function
 * Fiber Function Argument
 *
 * Temporary Prepare:
 * Entry Function
 *
 * Register file:
 * LR
 * FP
 * x28
 * x27
 * x26
 * x25
 * x24
 * x23
 * x22
 * x21
 * x20
 * x19
 * d15
 * d14
 * d13
 * d12
 * d11
 * d10
 * d9
 * d8
 */

namespace
{

constexpr size_t StackWordsReset = 1;
constexpr size_t StackWordsStorage = 2;
constexpr size_t StackWordsRegisters = 20;
constexpr size_t StackWordsAll = StackWordsStorage + StackWordsRegisters;
constexpr size_t StackSizeWord = sizeof(uintptr_t);
constexpr uintptr_t StackWatermark = 0xc0ffee'f00d'facade;

}

extern "C" void modm_context_entry() asm("modm_context_entry");
asm
(
	".globl modm_context_entry	\n\t"
	"modm_context_entry:		\n\t"
	"ldr x0, [sp]				\n\t" // Load closure data pointer
	"ldr x1, [sp, #8]			\n\t" // Load closure function
	"br x1						\n\t" // Jump to closure function
);


void
modm_context_init(modm_context_t *ctx,
				  uintptr_t *bottom, uintptr_t *top,
				  uintptr_t fn, uintptr_t fn_arg)
{
	ctx->bottom = bottom;
	ctx->top = top;

	ctx->sp = top;
	*--ctx->sp = fn;
	*--ctx->sp = fn_arg;
}

void
modm_context_reset(modm_context_t *ctx)
{
	*ctx->bottom = StackWatermark;

	ctx->sp = ctx->top - StackWordsStorage;
	*--ctx->sp = (uintptr_t) modm_context_entry;
	ctx->sp -= StackWordsRegisters - StackWordsReset;
}

void
modm_context_watermark(modm_context_t *ctx)
{
	// clear the register file on the stack
	for (auto *word = ctx->top - StackWordsAll;
		 word < ctx->top - StackWordsStorage - StackWordsReset; word++)
		*word = 0;

	// then color the whole stack *below* the register file
	for (auto *word = ctx->bottom; word < ctx->top - StackWordsAll; word++)
		*word = StackWatermark;
}

size_t
modm_context_stack_usage(const modm_context_t *ctx)
{
	for (auto *word = ctx->bottom; word < ctx->top; word++)
		if (StackWatermark != *word)
			return (ctx->top - word) * StackSizeWord;
	return 0;
}

bool
modm_context_stack_overflow(const modm_context_t *ctx)
{
	return *ctx->bottom != StackWatermark;
}

static modm_context_t main_context;

void
modm_context_start(modm_context_t *to)
{
	modm_context_jump(&main_context, to);
}

void
modm_context_end()
{
	modm_context_t dummy;
	modm_context_jump(&dummy, &main_context);
	__builtin_unreachable();
}

/*
The assembly code below is adapted from the Boost Context library to work
for Windows, Linux and macOS.
See https://github.com/boostorg/context/tree/develop/src/asm
- Windows: jump_arm64_aapcs_pe_armasm.asm
- Linux: jump_arm64_aapcs_elf_gas.S
- macOS: jump_arm64_aapcs_macho_gas.S

			Copyright Oliver Kowalke 2009.
   Distributed under the Boost Software License, Version 1.0.
	  (See accompanying file LICENSE_1_0.txt or copy at
			http://www.boost.org/LICENSE_1_0.txt)
*/

asm
(
	".globl modm_context_jump	\n\t"
	"modm_context_jump:			\n\t"

	/* move stack pointer down */
	"sub sp,  sp,  #0xa0		\n\t"

	/* save d8 - d15 */
	"stp d8,  d9,  [sp, #0x00]	\n\t"
	"stp d10, d11, [sp, #0x10]	\n\t"
	"stp d12, d13, [sp, #0x20]	\n\t"
	"stp d14, d15, [sp, #0x30]	\n\t"

	/* save x19-x30 */
	"stp x19, x20, [sp, #0x40]	\n\t"
	"stp x21, x22, [sp, #0x50]	\n\t"
	"stp x23, x24, [sp, #0x60]	\n\t"
	"stp x25, x26, [sp, #0x70]	\n\t"
	"stp x27, x28, [sp, #0x80]	\n\t"
	"stp fp,  lr,  [sp, #0x90]	\n\t"

	/* Store the SP in from->sp */
	"mov x19, sp				\n\t"
	"str x19, [x0]				\n\t"

	/* Restore SP from to->sp */
	"ldr x19, [x1]				\n\t"
	"mov sp,  x19				\n\t"

	/* load d8 - d15 */
	"ldp d8,  d9,  [sp, #0x00]	\n\t"
	"ldp d10, d11, [sp, #0x10]	\n\t"
	"ldp d12, d13, [sp, #0x20]	\n\t"
	"ldp d14, d15, [sp, #0x30]	\n\t"

	/* load x19-x30 */
	"ldp x19, x20, [sp, #0x40]	\n\t"
	"ldp x21, x22, [sp, #0x50]	\n\t"
	"ldp x23, x24, [sp, #0x60]	\n\t"
	"ldp x25, x26, [sp, #0x70]	\n\t"
	"ldp x27, x28, [sp, #0x80]	\n\t"
	"ldp fp,  lr,  [sp, #0x90]	\n\t"

	/* restore stack from GP + FPU */
	"add sp,  sp,  #0xa0		\n\t"

	"ret						\n\t"
);
