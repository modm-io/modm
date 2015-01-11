// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_INTERFACE_ADC_INTERRUPT_HPP
#define XPCC_INTERFACE_ADC_INTERRUPT_HPP

#include "adc.hpp"

namespace xpcc
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
 * @see AdcMaster	requires this class for a channel-mapped oversampling method.
 *
 * @author	Niklas Hauser
 * @ingroup adc
 */
class AdcInterrupt : public xpcc::Adc
{
protected:
	typedef void (*Handler) ();
#ifdef __DOXYGEN__
public:
	static inline void
	attachInterruptHandler(Handler handler=xpcc::dummy);
#endif
};

}	// namespace xpcc

#endif // XPCC_INTERFACE_ADC_INTERRUPT_HPP
