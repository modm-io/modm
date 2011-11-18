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
 * \file	irq.S
 * \brief	IRQ and FIQ handler
 * 
 */
// ----------------------------------------------------------------------------

#include "constants.h"

// Offsets of the registers relative to INTERRUPT_BASE_REGISTER (see constants.h)
#if defined(__ARM_AT91__)
#	define FIQ_ADDR			0x00000104		// AIC_FVR (address of FIQ handler)
#	define IRQ_ADDR			0x00000100		// AIC_IVR (address of IRQ handler)
#	define IRQ_END			0x00000130		// AIC_EOICR (marks end of IRQ interrupt)
#elif defined(__ARM_LPC2000__)
#	define FIQ_ADDR			0x00000030		// VIC_VectAddr
#	define IRQ_ADDR			0x00000030		// VIC_VectAddr
#	define IRQ_END			0x00000030		// VIC_VectAddr
#else
#	error	"Unknown device family: Please check the IRQ and FIQ handling "\
			"for this device and add the defines here!"
#endif
	
	// execute section in RAM
	.section .fastcode, "x"
	.arm
	
// ----------------------------------------------------------------------------
/**
 * @brief	FIQ Controller Interrupt Handler.
 * 
 * Calls AIC_FVR[interrupt] for the AT91SAM7S.
 * 
 * The FIQ interrupt can not be nested.
 * 
 * The registers r8-r14 are banked and don't need to be saved. r8 contains the
 * VIC Base Register (set during startup in startup.S). r9-r12 are used to
 * save r0-r3. Therefore the FIQ_STACK isn't used at all.
 * 
 * Stack usage:
 *      |                   | <--- svc_stack
 *      +-------------------+ 
 *      |     lr (r14)      | 
 *      +-------------------+ 
 *      |     ip (r12)      | <--- branch to interrupt handler routine from VIC
 *      +-------------------+ 
 *      |                   |
 * 
 * @author	Fabian Greif
 * @date	June 2011
 */
	.global _fiq_wrapper
_fiq_wrapper:
	// Save r0-r3 in FIQ_Register
	mov		r9, r0
	mov		r10, r1
	mov		r11, r2
	mov		r12, r3
	
	ldr		r0, [r8, #FIQ_ADDR]
	msr		CPSR_c, #ARM_MODE_SVC | ARM_IRQ_DISABLE | ARM_FIQ_DISABLE
	
	// Save scratch/used registers and LR in User Stack
	stmfd	sp!, { ip, lr }
	
	// Branch to the routine pointed by the AIC_FVR
	mov		lr, pc
	bx		r0
	
	// Restore scratch/used registers and LR from User Stack
	ldmfd	sp!, { ip, lr }
	
	// Leave Interrupts disabled and switch back in FIQ mode
	msr		CPSR_c, #ARM_MODE_FIQ | ARM_IRQ_DISABLE | ARM_FIQ_DISABLE
	
#ifdef __ARM_LPC2000__
	// Issue EOI command to the VIC
	ldr		r8, [r8, #FIQ_ADDR]
#endif
	
	// Restore r0-r3 which might be destroyed by the handler function register
	mov		r3, r12
	mov		r2, r11
	mov		r1, r10
	mov		r0, r9
	
	// Restore the Program Counter using the LR_fiq directly in the PC
	subs	pc, lr, #4

// ----------------------------------------------------------------------------
/**
 * @brief	IRQ Controller Interrupt Handler Wrapper.
 * 
 * Calls AIC_IVR[interrupt] for the AT91SAM7S.
 * 
 * The exception wrapper must be executed in ARM mode! This constraint does
 * not apply to the actual interrupt handler. That can be either ARM or
 * Thumb code.
 * 
 * The wrapper allows that interrupts are written as normal C functions for
 * the trade-off of some additional CPU cycles. It mainly saves and later
 * restores used registers (r0-r3 and r12).
 * 
 * A subroutine must preserve the content of the registers r4-r11 and
 * sp (r13), therefore the C interrupt handler functions is responsible
 * for restoring them. This is handled by the compiler, so you don't have
 * to worry about it.
 * 
 * Some registers have special roles:
 * r12 - ip (Intra-Procedure-call scratch register, must be saved)
 * r13 - sp (Stack pointer, banked)
 * r14 - lr (Link register, banked. Used as temporary register after saving it)
 * r15 - pc (program counter)
 * 
 * Stack Usage:
 * 
 *      |                   | <--- irq_stack
 *      +-------------------+ 
 *      |     lr (r14)      |
 *      +-------------------+ 
 *      |       SPSR        |
 *      +-------------------+ 
 *      |     ip (r12)      |
 *      +-------------------+ 
 *      |        r0         | <--- branch to svc_mode
 *      +-------------------+ 
 *      |                   | 
 *
 *
 *      |                   | <--- svc_stack
 *      +-------------------+ 
 *      |        r14        | 
 *      +-------------------+
 *      |        r3         |
 *      +-------------------+ 
 *      |        r2         |
 *      +-------------------+ 
 *      |        r1         | <--- branch to interrupt handler routine from VIC
 *      +-------------------+ 
 *      |                   |
 * 
 * @author	Fabian Greif
 * @date	June 2011
 */
	.global _irq_wrapper
_irq_wrapper:
	// Manage Exception Entry 
	// Adjust and save LR_irq in IRQ stack 
	sub		lr, lr, #4
	stmfd	sp!, {lr}
		
	// Push scratch/used registers and SPSR, needed for interrupt nesting
	mrs		lr, SPSR
	stmfd	sp!, {r0, ip, lr}
	
	// Get the ISR address pointed by the Interrupt controller
	ldr		lr, =INTERRUPT_BASE_REGISTER
	ldr		r0, [lr, #IRQ_ADDR]
#ifdef __ARM_AT91__
	// Write in the IVR to support Protect Mode (DBGM in AIC_DCR is set)
	// No effect in Normal Mode, de-assert the nIRQ and clear the source in
	// Protect Mode
	str		lr, [lr, #IRQ_ADDR]
#endif
	
	// Switch in Supervisor Mode and enable interrupts
	msr		CPSR_c, #ARM_MODE_SVC
	
	/* Call C Handler in SYSTEM mode.
	 * 
	 * All AAPCS registers used are saved to the IRQ or system stack.
	 * The handler must save and restore other registers (if used).
	 * 
	 * ARM-Thumb interworking is supported due to the "bx" instruction.
	 */ 
	stmfd	sp!, {r1-r3, lr}
	mov		lr, pc
	bx		r0
	ldmfd	sp!, {r1-r3, lr}
	
	// Switch back in IRQ mode and disable interrupts
	msr		CPSR_c, #ARM_MODE_IRQ | ARM_IRQ_DISABLE
	
	// Mark the End of Interrupt (EOI) on the Interrupt Controller
	ldr		lr, =INTERRUPT_BASE_REGISTER
	str		lr, [lr, #IRQ_END]
	
	// Restore SPSR_irq and r0 from IRQ stack
	ldmfd	sp!, {r0, ip, lr}
	msr		SPSR_cxsf, lr
	
	// Restore adjusted  LR_irq from IRQ stack directly in the PC
	ldmfd	sp!, {pc}^
	
// ----------------------------------------------------------------------------
/**
 * @brief	Save interrupt state and disable interrupts
 * 
 * C equivalent:
 * \code
 * extern "C" uint32_t _disable_interrupt(void);
 * \endcode
 * 
 * While the processor is executing the 'msr CPSR_cxsf, r1' instruction, the
 * active interrupts are disabled only on the next clock cycle. If an IRQ or
 * FIQ interrupt occurs during the execution of this instruction the bit IRQ or
 * FIQ is set both in the CPSR and in the SPSR (Saved Program Status Register)
 * and the processor enters the interrupt handler routine.
 * 
 * It must be taken care that the IRQ or FIQ bit in the SPSR are not modifyed
 * before exiting from the interrupt service routine. The _irq_wrapper and
 * _fiq_wrapper are not modifing the SPSR register, so this should not happen
 * here.
 * 
 * See Application Note "Disabling Interrupts at Processor Level"
 * 
 * @author	Fabian Greif
 * @date	June 2011
 */
	.global	_disable_interrupt
	.type	_disable_interrupt, %function
	.func	_disable_interrupt
_disable_interrupt:
	mrs		r0,	CPSR
	orr		r1, r0, #ARM_IRQ_DISABLE | ARM_FIQ_DISABLE
	msr		CPSR_cxsf, r1
	bx		lr
	.endfunc

/**
 * @brief	Save interrupt state and enable interrupts
 * 
 * C equivalent:
 * \code
 * extern "C" uint32_t _enable_interrupt(void);
 * \endcode
 * 
 * @author	Fabian Greif
 * @date	June 2011
 */
	.global	_enable_interrupt
	.type	_enable_interrupt, %function
	.func	_enable_interrupt
_enable_interrupt:
	mrs		r0,	CPSR
	bic		r1, r0, #ARM_IRQ_DISABLE | ARM_FIQ_DISABLE
	msr		CPSR_cxsf, r1
	bx		lr
	.endfunc

// ----------------------------------------------------------------------------	
/**
 * @brief	Restore interrupt state
 * 
 * C equivalent:
 * \code
 * extern "C" void _restore_interrupt(uint32_t cpsr);
 * \endcode
 * 
 * @author	Fabian Greif
 * @date	June 2011
 */
	.global	_restore_interrupt
	.type	_restore_interrupt, %function
	.func	_restore_interrupt
_restore_interrupt:
	msr		CPSR_c, r0
	bx		lr
	.endfunc
	
	.end
