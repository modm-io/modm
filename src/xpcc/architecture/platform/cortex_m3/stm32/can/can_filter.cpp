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

#include "can_filter.hpp"

// ----------------------------------------------------------------------------
void
xpcc::stm32::CanFilter::setFilterBase(uint8_t bank, uint32_t flags, uint32_t fr1, uint32_t fr2)
{
	uint32_t mask = (1UL << bank);
	
	// Initialization mode for the filter
	CAN1->FMR |= CAN_FMR_FINIT;
	
	// Disable filter
	CAN1->FA1R &= ~mask;
	
	if (flags & LIST_MODE) {
		CAN1->FM1R |= mask;
	}
	else {
		CAN1->FM1R &= ~mask;
	}
	
	if (flags & SINGLE_MODE) {
		CAN1->FS1R |= mask;
	}
	else {
		CAN1->FS1R &= ~mask;
	}
	
	if (flags & FIFO1) {
		CAN1->FFA1R |= mask;
	}
	else {
		CAN1->FFA1R &= ~mask;
	}
	
	CAN1->sFilterRegister[bank].FR1 = fr1;
	CAN1->sFilterRegister[bank].FR2 = fr2;
	
	// re-enable filter
	CAN1->FA1R |= mask;
	
	// Leave the initialization mode for the filter
	CAN1->FMR &= ~CAN_FMR_FINIT;
}

void
xpcc::stm32::CanFilter::disableFilter(uint8_t id)
{
	uint32_t mask = (1UL << id);
	
	// Initialization mode for the filter
	CAN1->FMR |= CAN_FMR_FINIT;
	
	// Disable filter
	CAN1->FA1R &= ~mask;
	
	// Leave the initialization mode for the filter
	CAN1->FMR &= ~CAN_FMR_FINIT;
}

// ----------------------------------------------------------------------------
#if defined(STM32F10X_CL)
void
xpcc::stm32::CanFilter::setStartFilterBankForCan2(uint8_t startBank)
{
	// Initialization mode for the filter
	CAN1->FMR |= CAN_FMR_FINIT;
	
	CAN1->FMR = (CAN1->FMR & ~0x3f00) | (startBank << 8);
	
	// Leave the initialization mode for the filter
	CAN1->FMR &= ~CAN_FMR_FINIT;
}
#endif
