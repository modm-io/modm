// coding: utf-8
/* Copyright (c) 2011-2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
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
