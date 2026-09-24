/*
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
#include <avr/io.h>

/* Stack layout (growing downwards):
 *
 * Permanent Storage:
 * Fiber Function
 * Fiber Function Argument
 *
 * Temporary Prepare:
 * Entry Function
 *
 * Register file: saves callee registers r2-r17, r28-r29
 *
 * return address (2-3 bytes)
 * r2
 * r3
 * r4
 * r5
 * r6
 * r7
 * r8
 * r9
 * r10
 * r11
 * r12
 * r13
 * r14
 * r15
 * r16
 * r17
 * r28
 * r29
 */

namespace
{

#ifdef __AVR_3_BYTE_PC__
static constexpr size_t StackWordsPc = 3;
#else
static constexpr size_t StackWordsPc = 2;
#endif
constexpr size_t StackWordsReset = StackWordsPc;
constexpr size_t StackWordsStorage = 4;
constexpr size_t StackWordsRegisters = 18 + StackWordsPc;
constexpr size_t StackWordsAll = StackWordsStorage + StackWordsRegisters;
constexpr size_t StackSizeWord = sizeof(uint8_t);
constexpr uint8_t StackWatermark = 0xaa;

void modm_naked
modm_context_entry()
{
	asm volatile(
		// pop function argument
		"pop r25	\n\t"
		"pop r24	\n\t"
		// pop function pointer
		"pop ZH		\n\t"
		"pop ZL		\n\t"

		// push function pointer back
		"push ZL	\n\t"
		"push ZH	\n\t"
		// push function argument back
		"push r24	\n\t"
		"push r25	\n\t"

		// jump to function
		"ijmp		\n\t"
	);
}

}

// The native stack size is uint8_t, not uintptr_t (=uint16_t)!
#define p2u(...) reinterpret_cast<uint8_t*>(__VA_ARGS__)
#define u2p(...) reinterpret_cast<uintptr_t*>(__VA_ARGS__)

void
modm_context_init(modm_context_t *ctx,
				  uintptr_t *bottom, uintptr_t *top,
				  uintptr_t fn, uintptr_t fn_arg)
{
	ctx->bottom = bottom;
	ctx->top = top;

	uint8_t *sp = p2u(top);
	// return address for modm_context_entry ret
	*--sp = uint16_t(fn);
	*--sp = uint16_t(fn) >> 8;
	// Argument 1
	*--sp = uint16_t(fn_arg);
	*--sp = uint16_t(fn_arg) >> 8;

	ctx->sp = u2p(sp);
}

void
modm_context_reset(modm_context_t *ctx)
{
	*p2u(ctx->bottom) = StackWatermark;

	uint8_t *sp = p2u(ctx->top) - StackWordsStorage;
	*--sp = uint16_t(modm_context_entry);
	*--sp = uint16_t(modm_context_entry) >> 8;
#ifdef __AVR_3_BYTE_PC__
	*--sp = 0;
#endif
	// saved register context r2-r17, r28-r29. We must +1 since the pop
	// instruction decrements the stack pointer before reading!
	sp -= StackWordsRegisters - StackWordsReset + 1;
	ctx->sp = u2p(sp);
}

void
modm_context_stack_watermark(modm_context_t *ctx)
{
	// clear the register file on the stack
	for (auto *word = p2u(ctx->top) - StackWordsAll;
		 word < p2u(ctx->top) - StackWordsStorage - StackWordsReset; word++)
		*word = 0;

	// then color the whole stack *below* the register file
	for (auto *word = p2u(ctx->bottom); word < p2u(ctx->top) - StackWordsAll; word++)
		*word = StackWatermark;
}

size_t
modm_context_stack_usage(const modm_context_t *ctx)
{
	for (auto *word = p2u(ctx->bottom); word < p2u(ctx->top); word++)
		if (StackWatermark != *word)
			return (p2u(ctx->top) - word) * StackSizeWord;
	return 0;
}

size_t
modm_context_stack_size(const modm_context_t *ctx)
{
	return (ctx->top - ctx->bottom) * StackSizeWord;
}

extern "C" uintptr_t modm_context_jump_entry(modm_context_t*, modm_context_t*);
extern "C" void modm_context_jump_return(uintptr_t, modm_context_t*);
static uintptr_t main_context_sp;

uintptr_t
modm_context_start(modm_context_t *to)
{
	return modm_context_jump_entry((modm_context_t*)&main_context_sp, to);
}

void modm_used
modm_context_end(uintptr_t retval)
{
	modm_context_jump_return(retval, (modm_context_t*)&main_context_sp);
	__builtin_unreachable();
}

#define MODM_PUSH_CONTEXT() \
		".irp regs, 2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,28,29	\n\t" \
		"push r\\regs 					\n\t" \
		".endr 							\n\t"

#define MODM_POP_CONTEXT() \
		".irp regs, 29,28,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2	\n\t" \
		"pop r\\regs			\n\t" \
		".endr					\n\t"

static_assert(offsetof(modm_context_t, sp) == 0);
static_assert(offsetof(modm_context_t, bottom) == 2);

uintptr_t modm_naked
modm_context_jump_entry(modm_context_t*, modm_context_t*)
{
	asm volatile
	(
		MODM_PUSH_CONTEXT()

		// Store the SP of current fiber
		"mov ZL, r24					\n\t"
		"mov ZH, r25					\n\t"
		"in	YL, __SP_L__				\n\t"
		"in	YH, __SP_H__				\n\t"
		"std Z+0, YL					\n\t"
		"std Z+1, YH					\n\t"

		"jmp modm_context_jump_return	\n\t"
	);
}

void modm_naked
modm_context_jump(modm_context_t*, modm_context_t*)
{
	asm volatile
	(
		MODM_PUSH_CONTEXT()

		// Store the SP of current fiber
		"mov ZL, r24			\n\t"
		"mov ZH, r25			\n\t"
		"in	YL, __SP_L__		\n\t"
		"in	YH, __SP_H__		\n\t"
		"std Z+0, YL			\n\t"
		"std Z+1, YH			\n\t"

		// Check for stack overflow via sp < from->bottom
		"ldd XL, Z+2			\n\t"
		"ldd XH, Z+3			\n\t"
		"cp  YL, XL				\n\t"
		"cpc YH, XH				\n\t"
		"brlo 1f				\n\t"

		// Check for stack overflow via *from->bottom != watermark
		"ld r20, X				\n\t"
		"cpi r20, %0			\n\t"
		"brne 1f				\n\t"

		// r24:r25 is unmodified, thus can be used to pass a return value
	"modm_context_jump_return:	\n\t"

		// Load the SP of next fiber
		"mov ZL, r22			\n\t"
		"mov ZH, r23			\n\t"
		"ldd r22, Z+0			\n\t"
		"ldd r23, Z+1			\n\t"

		// Save SREG and disable interrupts
		"in r20, __SREG__		\n\t"
		"cli					\n\t"

		// Write the SP
		"out __SP_L__, r22		\n\t"
		"out __SP_H__, r23		\n\t"

		// Re-enable interrupts by restoring SREG
		"out __SREG__, r20		\n\t"

		MODM_POP_CONTEXT()

		"ret					\n\t"

	"1:  jmp modm_context_end	\n\t"

		:: "i" (StackWatermark)
	);
}
