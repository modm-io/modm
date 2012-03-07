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
 * $Id: constants.h 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------
/**
 * \file	constants.h
 * \brief	Register description and constants
 * 
 * This file will be included by C and Assembler code. Therefore it must
 * only contain Preprocessor directives.
 * 
 * When called from Assembler the __ASSEMBLER__ define is available.
 */

#ifndef XPCC__CONSTANTS_H
#define XPCC__CONSTANTS_H

/*
 * ARM register quick reference:
 * 
 * Name    Number   ARM Procedure Calling Standard Role
 * 
 * a1      r0       argument 1 / integer result / scratch register / argc
 * a2      r1       argument 2 / scratch register / argv
 * a3      r2       argument 3 / scratch register / envp
 * a4      r3       argument 4 / scratch register
 * v1      r4       register variable
 * v2      r5       register variable
 * v3      r6       register variable
 * v4      r7       register variable
 * v5      r8       register variable
 * sb/v6   r9       static base / register variable
 * sl/v7   r10      stack limit / stack chunk handle /  reg. variable
 * fp      r11      frame pointer
 * ip      r12      scratch register / new-sb in inter-link-unit calls
 * sp      r13      lower end of current stack frame
 * lr      r14      link address / scratch register
 * pc      r15      program counter
 */

/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs */
#define ARM_MODE_MASK		0x1F	/* lower 5 bits of CPSR */

#define ARM_MODE_USR		0x10	// User Mode
#define ARM_MODE_FIQ		0x11	// Fast Interrupt Mode
#define ARM_MODE_IRQ		0x12	// Interrupt Mode
#define ARM_MODE_SVC		0x13	// Supervisor Mode
#define ARM_MODE_ABT		0x17	// Abort Mode
#define ARM_MODE_UND		0x1B	// Unidentified Instruction Mode
#define ARM_MODE_SYS		0x1F	// System Mode (uses the same Stack as User Mode)

#define ARM_CPSR_N_BIT		(1 << 31)
#define ARM_CPSR_Z_BIT		(1 << 30)
#define ARM_CPSR_C_BIT		(1 << 29)
#define ARM_CPSR_V_BIT		(1 << 28)
#define ARM_CPSR_I_BIT		(1 << 7)
#define ARM_CPSR_F_BIT		(1 << 6)
#define ARM_CPSR_T_BIT		(1 << 5)

#define ARM_IRQ_DISABLE		(1 << 7)	// when I bit is set, IRQ is disabled
#define ARM_FIQ_DISABLE		(1 << 6)	// when F bit is set, FIQ is disabled
#define ARM_THUMB_ENABLE 	(1 << 5)	// when T bit is set, Thumb mode is enabled

// constant to pre-fill the stack
#define	STACK_FILL			0xaaaaaaaa
#define	MAGIC_MARKER		0xdeadbeef

// Registers
#if defined(__ARM_AT91__) || defined(__ARM_LPC2000__)
#	define INTERRUPT_BASE_REGISTER	0xFFFFF000
#else
#	error "Unknown device family: Check the value of VIC_BASE_REGISTER"
#endif

#endif	// XPCC__CONSTANTS_H
