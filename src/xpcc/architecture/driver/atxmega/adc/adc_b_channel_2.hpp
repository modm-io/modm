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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file.
 */
// ----------------------------------------------------------------------------


#ifndef XPCC__XMEGA_ADCB_CHANNEL2_HPP
#define XPCC__XMEGA_ADCB_CHANNEL2_HPP

#include <avr/io.h>
#include <stdint.h>

#include <xpcc/architecture/platform.hpp>
#include "adc_b.hpp"

#if defined(ADCB) || defined(__DOXYGEN__)

namespace xpcc
{
	/**
	 * \brief		ADC Channel 2 of Module B
	 * 
	 * \ingroup		atxmega_adc
	 */
	class AdcChannelB2 : public AdcModuleB
	{
	public:
		inline static ADC_CH_t&
		getChannelBase()
		{
			return ADCB.CH2;
		}
		
		/// Select between single ended, differential (with gain)
		inline static void
		setInputMode(ADC_CH_INPUTMODE_t mode=ADC_CH_INPUTMODE_SINGLEENDED_gc)
		{
			ADCB_CH2_CTRL = (ADCB_CH2_CTRL & ~ADC_CH_INPUTMODE_gm) | mode;
		}
		
		/// Select a gain factor for the channel
		inline static void
		setGainFactor(ADC_CH_GAIN_t factor=ADC_CH_GAIN_1X_gc)
		{
			ADCB_CH2_CTRL = (ADCB_CH2_CTRL & ~ADC_CH_GAINFAC_gm) | factor;
		}
		
		/// Select the pos. and neg. input pins for the channel
		inline static void
		selectInput(uint8_t selection)
		{
			ADCB_CH2_MUXCTRL = selection;
		}
		
		/**
		 * \brief	 Enable Interrupt
		 * 
		 * If you enable the ADC channel interrupt you need to provide
		 * a corresponding interrupt handler function. Otherwise the
		 * controller will restart on every invocation of the interrupt.
		 * 
		 * \code
		 * ISR(ADCB_CH2_vect)
		 * {
		 *     ...
		 * }
		 * \endcode
		 */
		inline static void
		enableInterrupt(uint8_t level=ADC_CH_INTLVL_OFF_gc)
		{
			ADCB_CH2_INTCTRL = level;
		}
		
		/**
		 * \brief	Read the value off the analog channel
		 * 
		 * Blocks until the conversion is done.
		 * 
		 * Equivalent to:
		 * \code
		 * startConversion();
		 * while (!isFinished())
		 *     ;
		 * getResult();
		 * \endcode
		 */
		static uint16_t
		read();
		
		static void
		startConversion();
		
		inline static bool
		isFinished()
		{
			return (ADCB_CH2_INTFLAGS & ADC_CH_CHIF_bm);
		}
		
		/// Read the converted analog value
		inline static uint16_t
		getValue()
		{
			return ADCB_CH2_RES;
		}
	};
}

#endif	// ADCB
#endif // XPCC__XMEGA_ADCB_CHANNEL2_HPP
