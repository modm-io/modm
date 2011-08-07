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


#ifndef XPCC_ATXMEGA__ADCA_CHANNEL0_HPP
#define XPCC_ATXMEGA__ADCA_CHANNEL0_HPP

#include <avr/io.h>
#include <stdint.h>

#include "adc_a.hpp"

#if defined(ADCA) || defined(__DOXYGEN__)

namespace xpcc
{
	namespace atxmega
	{
		/**
		 * \brief		ADC Channel 0 of Module A
		 * 
		 * \warning	Every channel must either be in unsigned mode (internal or
		 * 			single-ended conversion) or in signed mode (differential or
		 * 			differential conversion with gain). It is not possible to
		 * 			mix these two within one ADC module!
		 * 
		 * \ingroup		atxmega_adc
		 */
		class AdcChannelA0 : public AdcModuleA
		{
		public:
			inline static ADC_CH_t&
			getChannelRegister()
			{
				return ADCA.CH0;
			}
			
			/**
			 * \brief	Read internal inputs
			 * 
			 * The Xmegas have four internal analog signals that can be selected
			 * as input and be measured by the ADC: 
			 * - Temperature sensor (ADC_CH_MUXINT_TEMP_gc)
			 * - Bandgap voltage (ADC_CH_MUXINT_BANDGAP_gc)
			 * - VCC scaled (1/10)	(ADC_CH_MUXINT_SCALEDVCC_gc)
			 * - DAC output (ADC_CH_MUXINT_DAC_gc)
			 * 
			 * \param	input	Internal input
			 */
			static void
			setInternalInputMode(ADC_CH_MUXINT_t input);
			
			/**
			 * \brief	Set channel to single ended conversion mode
			 * 
			 * \param	input	Input pin (e.g. ADC_CH_MUXPOS_PIN0_gc)
			 */
			static void
			setSingleEndedMode(ADC_CH_MUXPOS_t input);
			
			/**
			 * \brief	Selects differential input and a signed conversion mode.
			 * 
			 * \param	positiveInput	Positive input pin (e.g. ADC_CH_MUXPOS_PIN0_gc)
			 * \param	negativeInput	Negative input pin (e.g. ADC_CH_MUXNEG_PIN0_gc)
			 * 
			 * \warning	This also selects signed conversion mode for the complete ADC module! 
			 */
			static void
			setDifferentialMode(
					ADC_CH_MUXPOS_t positiveInput, ADC_CH_MUXNEG_t negativeInput);
			
			/**
			 * \brief	Selects differential input and a signed conversion mode.
			 * 
			 * Gain will decrease resolution.
			 * 
			 * \warning	This also selects signed conversion mode for the complete ADC module!
			 */
			static void
			setDifferentialGainMode(
					ADC_CH_MUXPOS_t positiveInput, ADC_CH_MUXNEG_t negativeInput,
					ADC_CH_GAIN_t gainFactor = ADC_CH_GAIN_1X_gc);
			
			/**
			 * \brief	 Enable Interrupt
			 * 
			 * If you enable the ADC channel interrupt you need to provide
			 * a corresponding interrupt handler function. Otherwise the
			 * controller might restart on every invocation of the interrupt.
			 * 
			 * \code
			 * ISR(ADCA_CH0_vect)
			 * {
			 *     ...
			 * }
			 * \endcode
			 */
			inline static void
			enableInterrupt(ADC_CH_INTLVL_t level=ADC_CH_INTLVL_OFF_gc)
			{
				ADCA_CH0_INTCTRL = level;
			}
			
			/**
			 * \brief	Starts one single conversion.
			 * 
			 * \code
			 * f_adc = f_peripheral / prescaler
			 * t_propagation = (1 + RES / 2 + GAIN) / f_adc
			 * 
			 * RES = 8 or 12
			 * GAIN = 1 if differential mode mit gain is used, 0 otherwise
			 * \endcode
			 */
			static inline void
			startConversion()
			{
				// reset flag
				ADCA_CH0_INTFLAGS |= ADC_CH_CHIF_bm;
				
				// start the new conversion
				ADCA_CH0_CTRL |= ADC_CH_START_bm;
			}
			
			/**
			 * \brief	Check if the conversion is finished
			 * \return \c true if the conversion finished.
			 */
			inline static bool
			isFinished()
			{
				return (ADCA_CH0_INTFLAGS & ADC_CH_CHIF_bm);
			}
			
			/// Read the last converted analog value
			inline static uint16_t
			getResult()
			{
				return ADCA_CH0_RES;
			}
			
			/**
			 * \brief	Read the value off the analog channel (convenience function)
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
		};
	}
}

#endif // ADCA
#endif // XPCC_ATXMEGA__ADCA_CHANNEL0_HPP
