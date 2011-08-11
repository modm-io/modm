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


#ifndef XPCC__ATMEGA_ADC_INTERRUPT_HPP
#define XPCC__ATMEGA_ADC_INTERRUPT_HPP

#include <avr/io.h>
#include <stdint.h>
#include <xpcc/utils/dummy.hpp>

#include "adc.hpp"

namespace xpcc
{
	namespace atmega
	{
		/**
		 * \brief		ADC Interrupt Module
		 *
 		 * \ingroup		atmega
		 * \author		Niklas Hauser
		 */
		class AdcInterrupt : public Adc
		{
		public:
			typedef void (*F) ();
			
			static inline void
			setAutoTriggerSource(uint8_t source)
			{
				ADCSRB = source;
			}
			
			static inline void
			setAutoTrigger(bool enable)
			{
				if (enable)
					ADCSRA |= (1 << ADATE);
				else
					ADCSRA &= ~(1 << ADATE);
			}
			
			static inline void
			setInterrupt(bool enable)
			{
				if (enable)
					ADCSRA |= (1 << ADIE);
				else
					ADCSRA &= ~(1 << ADIE);
			}
			
			static inline void
			setChannel(uint8_t channel)
			{
				if (channel > 8)
					return;
				ADMUX = (ADMUX & 0xe0) | channel;
			}
			
			static inline void
			attachConversionCompleteInterrupt(F function=xpcc::dummy)
			{
				conversionComplete = function;
			}
			
		public:
			static F conversionComplete;
		};
	}
}

#endif // XPCC__ATMEGA_ADC_INTERRUPT_HPP
