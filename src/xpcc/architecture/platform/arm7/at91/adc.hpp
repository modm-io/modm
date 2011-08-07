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

#ifndef XPCC_AT91__ADC_HPP
#define XPCC_AT91__ADC_HPP

#include <stdint.h>

#include "aic.hpp"
#include "device.h"

namespace xpcc
{
	namespace at91
	{
		/**
		 * @brief	Analog-to-Digital Converter (ADC)
		 * 
		 * The ADC Controller clock (MCK) is always clocked.
		 * 
		 * The analog input pins can be multiplexed with PIO lines. In this
		 * case, the assignment of the ADC input is automatically done as soon
		 * as the corresponding channel is enabled. By default, after reset,
		 * the PIO line is configured as input with its pull-up enabled and the
		 * ADC input is connected to the GND.
		 * 
		 * @date	July 2011
		 * @ingroup	at91
		 * @author	Fabian Greif
		 */
		class Adc
		{
		public:
			enum Channel
			{
				CHANNEL_0 = 0,
				CHANNEL_1 = 1,
				CHANNEL_2 = 2,
				CHANNEL_3 = 3,
				CHANNEL_4 = 4,
				CHANNEL_5 = 5,
				CHANNEL_6 = 6,
				CHANNEL_7 = 7,
			};
		public:
			/**
			 * @brief	Initialize A/D converter
			 * 
			 * f_adc = MCK / ( (PRESCAL + 1) * 2 )
			 * Startup Time = (STARTUP+1) * 8 / f_adc
			 * Sample & Hold Time = SHTIM / f_adc
			 * 
			 * @param	adcClock 			ADC clock in Hz (default = 5 MHz)
			 * @param	startupTime			Start up time when returning from
			 * 								Idle Mode in µs (default = 20µs)
			 * @param	sampleAndHoldTime	Sample and hold time in ns (default = 600ns)
			 * 
			 * Maximum clock frequency for 10-bit resolution is 5 MHz and
			 * 8 Mhz for 8-bit resolution. This equals a throughput rate of
			 * 384 or 533 kSPS (3 (5 for 8 Mhz) clock cycles for track and hold
			 * acquisition time and 10 clock cycles for conversion).
			 */
			static inline void
			initialize(
					uint32_t adcClock = 5e6,
					uint32_t startupTime = 20,
					uint32_t sampleAndHoldTime = 600)
			{
				const uint32_t prescal = (F_CPU / (2 * adcClock)) - 1;
				const uint32_t startup = ((adcClock / 1000000) * startupTime / 8) - 1;
				const uint32_t shtim   = (((adcClock / 1000000) * sampleAndHoldTime) / 1000);
				
				// Perform a software reset for the ADC
				AT91C_BASE_ADC->ADC_CR = AT91C_ADC_SWRST;
				
				// No hardware triggers (yet)
				AT91C_BASE_ADC->ADC_MR = 
						((prescal << 8) & AT91C_ADC_PRESCAL) |
						((startup << 16) & AT91C_ADC_STARTUP) | 
						((shtim << 24) & AT91C_ADC_SHTIM); 
			}
			
			/**
			 * @brief	Enable channel
			 * @param	channel	[0..7]
			 */
			static void
			enableChannel(uint8_t channel);
			
			/**
			 * @brief	Disabled channel
			 * 
			 * @param	channel	[0..7]
			 * @warning	If the corresponding channel is disabled during a
			 * 			conversion or if it is disabled then reenabled during
			 * 			a conversion, its associated data and its corresponding
			 * 			flags are unpredictable.
			 */
			static void
			disableChannel(uint8_t channel);
			
			/**
			 * @brief	Starts the conversion on all enabled channels
			 */
			static void
			startConversion();
			
			/**
			 * @brief	Check if the conversion is finished
			 * 
			 * Flag is reset by a read of getValue().
			 * 
			 * @see		getValue()
			 */
			static bool
			isFinished(uint8_t channel);
			
			/**
			 * @brief	Check if all conversion of the currently enabled
			 * 			channels are finished
			 * 
			 * Flag is reset is \b one of the channels is read (via getValue()).
			 * 
			 * @see		getValue()
			 */
			static bool
			isFinished();
			
			/**
			 * @brief	Get result of a conversion
			 * 
			 * @param	channel		Channel number [0..7]
			 * @return	10-Bit conversion result. The remaining upper 22-bit
			 * 			will always be zero. The result is valid until the
			 * 			next conversion.
			 */
			static uint32_t
			getValue(uint8_t channel);
			
			static uint32_t
			getLastConvertedValue();
			
			static void
			enableInterrupt(uint8_t channel);
			
			static void
			disableInterrupt(uint8_t channel);
			
			/**
			 * @brief	Set a interrupt handler function
			 * 
			 * The handler function needs to read PIT_PIVR in order to
			 * acknowledge the interrupt.
			 */
			static void
			registerInterruptHandler(Aic::Handler function);
			
			/**
			 * @brief	Remove interrupt handler and disable the interrupt
			 */
			static void
			removeInterruptHandler();
		};
	}
}

#endif // XPCC_AT91__ADC_HPP
