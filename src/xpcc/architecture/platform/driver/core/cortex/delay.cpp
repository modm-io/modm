// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "../../../device.hpp"
#include "../../clock/generic/common_clock.hpp"

extern "C"
{

#define DWT_CYCCNT      (*((__IO uint32_t *)0xE0001004))

void ATTRIBUTE_FASTCODE
_delay_us(uint16_t us)
{
	uint32_t start = DWT_CYCCNT;
	// prefer this for cores with fast hardware multiplication
	int32_t delay = int32_t(xpcc::clock::fcpu_MHz) * us - 35;

	while (int32_t(DWT_CYCCNT - start) < delay)
		;
}

void ATTRIBUTE_FASTCODE
_delay_ms(uint16_t ms)
{
	if (!ms) return;

	uint32_t start = DWT_CYCCNT;
	int32_t delay = int32_t(xpcc::clock::fcpu_kHz) * ms - 35;

	while (int32_t(DWT_CYCCNT - start) < delay)
		;
}

}
