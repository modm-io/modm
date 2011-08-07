// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------
/**
 * @file	abort.S
 * @ingroup	at91
 * 
 * @brief	Abort Handler Wrapper
 * 
 * These handlers are called directly form the vector table and save the abort
 * cause and the corrected return address and branch to a common handler.
 * This common handler collectes the remaining context of the currently
 * executed mode.
 * 
 * To display a report (via UART) a C function called _abort_handler() is
 * called with a pointer to the abort context as first parameter.
 * 
 * By default an abort is expected to be an unrecoverable program fault,
 * therefore this function must not return. It it does the CPU operation is
 * stopped by disabling interrupts and executing an infinite loop.
 * 
 * In production code the default _abort_handler() may be replaced by a
 * function to restart the processor in the hope that the abort was caused
 * by a temporary fault. 
 * 
 *        MEMORY MAP
 * 
 *     |              | <-- Top of Abort Stack
 *     +--------------+ 
 *  22 |     PSW      |
 *     +--------------+ 
 *  21 |   abort_id   |
 *     +--------------+
 *  20 |      PC      |
 *     +--------------+
 *  19 |     r15      |
 *     +--------------+
 *  18 |     r14      |
 *     +--------------+
 *     |     ...      |
 *     +--------------+
 *   5 |      r1      |
 *     +--------------+
 *   4 |      r0      | 
 *     +--------------+ 
 *   3 |    sp_svc    |
 *     +--------------+
 *   2 |    sp_irq    |
 *     +--------------+
 *   1 |    sp_fiq    |
 *     +--------------+
 *   0 |    sp_usr    | <-- Argument 0 for _abort_handler()
 *     +--------------+
 * 
 * There are three possible values for \c abort_id:
 * 
 * - 0 : Undefined instruction
 * - 1 : Prefetch Abort
 * - 2 : Data Abort
 * 
 * They all are different manifestations of similar problems. A data abort
 * occurs when a write or read access to an undefined area apears (marked as
 * Reserved Address Space or Undefined Area in the memory mapping overview
 * in the datasheet).
 * This happens mostly because of an invalid pointer. These errors are easy
 * to find as the Program Counter points directly to the line in which the
 * invalid access occured. Trace back where the data came from an the error
 * should be located.
 * 
 * More problematic are Undefined Instruction and Prefetch Aborts. These may
 * happen due to an invalid function pointer or an overwritten return address.
 * In this case the PC is loaded with either an invalid address (prefetch abort)
 * or the core finds no valid instruction at the current address (undefined
 * instruction).
 * The problem here is that the PC is already wrong, therefore it is difficult
 * to locate the point of origin. A good point to start is to check the link
 * register (lr/r14) to see where the function came from. The content of the
 * other registers might provide clues to.
 * 
 * Happy debugging ;-)
 * 
 * @author	Fabian Greif
 * @date	June 2011
 */
// ----------------------------------------------------------------------------
	
	#include "constants.h"
	
	.text
	.arm
	
	.global _undefined_instruction_handler
	.global _prefetch_abort_handler
	.global _data_abort_handler
	
// ----------------------------------------------------------------------------
_undefined_instruction_handler:
	sub		sp, sp, #4 * 4
	stmia	sp!, {lr}
	sub		lr, lr, #4		/* ajust lr to point to the originating command */
	stmia	sp!, {lr}
	mov		lr, #0x0
	b		common_handler
	
_prefetch_abort_handler:
	sub		sp, sp, #4 * 4
	stmia	sp!, {lr}
	sub		lr, lr, #4		/* ajust lr to point to the originating command */
	stmia	sp!, {lr}
	mov		lr, #0x1
	b		common_handler
	
_data_abort_handler:
	sub		sp, sp, #4 * 4
	stmia	sp!, {lr}
	/* ajust lr to point to the originating command.
	 * 
	 * The instruction that caused the abort is at lr_ABT - 8 because lr_ABT
	 * points two instructions beyond the instruction that caused the abort. */
	sub		lr, lr, #8
	stmia	sp!, {lr}
	mov		lr, #0x2
	b		common_handler

// ----------------------------------------------------------------------------
common_handler:
	stmia	sp!, {lr}
	mrs		lr, SPSR
	stmia	sp!, {lr}
	
	/* allocate space for current context and save the lower registers
	 * => lower registers are the same for all modes (not banked) */
	sub		sp, sp, #(19 * 4)
	stmia	sp, {r0-r7}
	
	/* save current mode in r0 and disable interrupts */
	mrs		r0, CPSR
	orr		r0, r0, #ARM_IRQ_DISABLE | ARM_FIQ_DISABLE
	msr		CPSR_c, r0
	
	/* save and ajust stackpointer before changing mode */
	add		r2, sp, #(8 * 4)
	mov		r3, sp
	
	/* Save r8-r14 from within originating mode (with interrupts disabled)
	 * to get the right value for banked registers */
	mrs		r1, SPSR
	tst		r1, #0xf
	orreq	r1, r1, #0xf				/* if mode is 'user' change to 'system' to stay in a privileged mode */
	orr		r1, r1, #ARM_IRQ_DISABLE | ARM_FIQ_DISABLE	/* disable IRQ and FIQ interrupt */
	bic		r1, r1, #ARM_THUMB_ENABLE	/* Thumb bit might be set, clear it => force ARM mode */
	msr		CPSR_c, r1
	stmia	r2, {r8-r14}
	
	/* Save current stack pointers form the different modes */
	msr		CPSR_c, #ARM_MODE_SVC | ARM_IRQ_DISABLE | ARM_FIQ_DISABLE
	stmfd	r3!, {sp}
	msr		CPSR_c, #ARM_MODE_IRQ | ARM_IRQ_DISABLE | ARM_FIQ_DISABLE
	stmfd	r3!, {sp}
	msr		CPSR_c, #ARM_MODE_FIQ | ARM_IRQ_DISABLE | ARM_FIQ_DISABLE
	stmfd	r3!, {sp}
	msr		CPSR_c, #ARM_MODE_SYS | ARM_IRQ_DISABLE | ARM_FIQ_DISABLE
	stmfd	r3!, {sp}
	
	msr		CPSR_c, r0				/* switch back */
	mov		sp, r3
	
	/* Call the exception handler to display the result.
	 * The handler should not return, go to an infinite loop if it does. */
	mov		r0, sp
	ldr		r1, =_abortHandler
	mov		lr, pc
	bx		r1
	b		.
	
	.end
