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
 */
// ----------------------------------------------------------------------------

#include <xpcc/debug/logger.hpp>

#include "constants.h"
#include "abort.h"

#include <xpcc_config.hpp>

#undef XPCC_LOG_LEVEL
#if ARM7_ENABLE_ABORT_HANDLER != 0
#	define XPCC_LOG_LEVEL	xpcc::log::ERROR
#else
// with logging disabled all calls to XPCC_LOG_ERROR are removed. The
// surrounding code does nothing then and a good compiler will remove it too.
// Only the infinite loop remains.
#	define XPCC_LOG_LEVEL	xpcc::log::DISABLED
#endif

// set by the linker-script
extern uint32_t __stack_start;
extern uint32_t __stack_end;

// from "syscalls.cpp", points to the current top of the heap
extern uint8_t *__brkval;

// ----------------------------------------------------------------------------
extern "C"
void
_abortHandler(const AbortContext* ctx)
{
	switch (ctx->abort_id)
	{
	case 0:
		XPCC_LOG_ERROR << "\n\nUndefined Instruction:\n";
		break;
	case 1:
		XPCC_LOG_ERROR << "\n\nPrefetch Abort:\n";
		break;
	case 2:
		XPCC_LOG_ERROR << "\n\nData Abort:\n";
		break;
	default:
		XPCC_LOG_ERROR << "\n\nUnknown Exception:\n";
		break;
	}
	
	// Step 1. Print a Register dump
	for (int i = 0; i < 8; i++)
	{
		XPCC_LOG_ERROR.printf("r%d : 0x%08lx   ", i, ctx->reg[i]);
		XPCC_LOG_ERROR.printf("r%d%s: 0x%08lx\n", i + 8, i + 8 < 10 ? " " : "", ctx->reg[i + 8]);
	}
	
	// Step 2. Print status flags
	XPCC_LOG_ERROR.printf("PSR: 0x%08lx  ", ctx->cpsr);
	
	// now decode the flags
	uint32_t cpsr = ctx->cpsr;
	XPCC_LOG_ERROR << ((cpsr & ARM_CPSR_N_BIT) ? 'N' : 'n');
	XPCC_LOG_ERROR << ((cpsr & ARM_CPSR_Z_BIT) ? 'Z' : 'z');
	XPCC_LOG_ERROR << ((cpsr & ARM_CPSR_C_BIT) ? 'C' : 'c');
	XPCC_LOG_ERROR << ((cpsr & ARM_CPSR_V_BIT) ? 'V' : 'v');
	XPCC_LOG_ERROR << "...";
	XPCC_LOG_ERROR << ((cpsr & ARM_CPSR_F_BIT) ? 'F' : 'f');
	XPCC_LOG_ERROR << ((cpsr & ARM_CPSR_I_BIT) ? 'I' : 'i');
	XPCC_LOG_ERROR << ((cpsr & ARM_CPSR_T_BIT) ? 'T' : 't');
	
	const char *mode;
	switch (ARM_MODE_MASK & cpsr)
	{
	case ARM_MODE_USR:
		mode = "user";
		break;
	case ARM_MODE_FIQ:
		mode = "fiq";
		break;
	case ARM_MODE_IRQ:
		mode = "irq";
		break;
	case ARM_MODE_SVC:
		mode = "supervisor";
		break;
	case ARM_MODE_SYS:
		mode = "system";
		break;
	default:
		mode = 0;
		break;
	}
	if (mode) {
		XPCC_LOG_ERROR.printf(", %s-mode\n", mode);
	}
	
	XPCC_LOG_ERROR << "\nStackpointer:\n";
	XPCC_LOG_ERROR.printf("usr:  0x%08lx\n", ctx->sp_usr);
	XPCC_LOG_ERROR.printf("fiq:  0x%08lx\n", ctx->sp_fiq);
	XPCC_LOG_ERROR.printf("irq:  0x%08lx\n", ctx->sp_irq);
	XPCC_LOG_ERROR.printf("svc:  0x%08lx\n", ctx->sp_svc);
	XPCC_LOG_ERROR.printf("abt:  0x%08lx\n", (uint32_t) ctx);
	XPCC_LOG_ERROR.printf("heap: 0x%08lx\n", (uint32_t) __brkval);
	
	// Step 3. Try to generate a Backtrace
	XPCC_LOG_ERROR << "\nBacktrace:\n";
	XPCC_LOG_ERROR.printf(" 0) 0x%08lx\n", ctx->pc);
	// TODO
	
	// Step 4. Print a RAM dump
	XPCC_LOG_ERROR << "\nStack dump:";
	uint32_t counter = -1;
	const uint32_t *ptr = &__stack_start;
	while (ptr != &__stack_end)
	{
		counter++;
		if ((counter % 8) == 0) {
			XPCC_LOG_ERROR.printf("\n0x%08lx :", (uint32_t) ptr);
		}
		XPCC_LOG_ERROR.printf(" %08lx", *ptr++);
	}
	
	// Infinite loop
	while (1)
	{
	}
}
