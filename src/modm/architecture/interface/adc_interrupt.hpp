/*
 * Copyright (c) 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_INTERFACE_ADC_INTERRUPT_HPP
#define MODM_INTERFACE_ADC_INTERRUPT_HPP

#include "adc.hpp"

namespace modm
{

/**
 * Basic interrupt-based interface of an ADC.
 *
 * This class allows you to attach functions to the ADC Conversion
 * Complete Interrupt via function pointers.
 * Be aware however, that this implementation is slower and requires
 * more resources than writing the function code directly into
 * the interrupt service routines.
 *
 * This interface allows for asynchronous operation:
 * @code
 * void interruptComplete()
 * {
 *     uint16_t value = Adc::getValue();
 * }
 *
 * AdcInterrupt::attachConversionCompleteInterrupt(interruptComplete);
 * AdcInterrupt::enableInterrupt();
 *
 * // start a new conversion on channel 3
 * AdcInterrupt::setChannel(3);
 * AdcInterrupt::startConversion();
 * @endcode
 *
 * @see Adc			for the synchronous method.
 *
 * @author	Niklas Hauser
 * @ingroup modm_architecture_adc
 */
class AdcInterrupt : public modm::Adc
{
protected:
	typedef void (*Handler) ();
#ifdef __DOXYGEN__
public:
	static inline void
	attachInterruptHandler(Handler handler=modm::dummy);
#endif
};

}	// namespace modm

#endif // MODM_INTERFACE_ADC_INTERRUPT_HPP
