// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "../../../device.hpp"

// this should be able to be generated instead of using Macros for this.
#ifdef ADC1
#	include "adc_interrupt_1.hpp"
#endif
#ifdef ADC2
#	include "adc_interrupt_2.hpp"
#endif
#ifdef ADC3
#	include "adc_interrupt_3.hpp"
#endif

extern "C" void
ADC_IRQHandler(void)
{

#ifdef ADC1
	if (xpcc::stm32::AdcInterrupt1::getInterruptFlags()) {
		xpcc::stm32::AdcInterrupt1::handler();
	}
#endif

#ifdef ADC2
	if (xpcc::stm32::AdcInterrupt2::getInterruptFlags()) {
		xpcc::stm32::AdcInterrupt2::handler();
	}
#endif

#ifdef ADC3
	if (xpcc::stm32::AdcInterrupt3::getInterruptFlags()) {
		xpcc::stm32::AdcInterrupt3::handler();
	}
#endif

}



