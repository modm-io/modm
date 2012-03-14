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

#ifndef XPCC_ATXMEGA__ADCB_CHANNEL2_HPP
#define XPCC_ATXMEGA__ADCB_CHANNEL2_HPP

#include <avr/io.h>
#include <stdint.h>
#include "adcb.hpp"

#if defined(ADCB) || defined(__DOXYGEN__)

namespace xpcc
{
	namespace atxmega
	{
		/**
		 * \brief		ADC Channel 2 of Module B
		 * 
		 * \warning	Every channel must either be in unsigned mode (internal or
		 * 			single-ended conversion) or in signed mode (differential or
		 * 			differential conversion with gain). It is not possible to
		 * 			mix these two within one ADC module!
		 * 
		 * \ingroup		atxmega_adc
		 */
		class AdcChannelB2 : public AdcB
		{
		public:
			// MARK: CTRL
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
			inline static void
			startSingleConversion()
			{
				// reset flag
				ADCB_CH2_INTFLAGS |= ADC_CH_CHIF_bm;
				
				// start the new conversion
				ADCB_CH2_CTRL |= ADC_CH_START_bm;
			}
			
			inline static void
			setGainFactor(adc::Gain gain=adc::GAIN_1)
			{
				ADCB_CH2_CTRL = (ADCB_CH2_CTRL & ~ADC_CH_GAINFAC_gm) | gain;
			}
			
			inline static void
			setInputMode(adc::InputMode mode=adc::INPUTMODE_INTERNAL)
			{
				setSignedConversion((mode >= adc::INPUTMODE_DIFFERENTIAL));
				ADCB_CH2_CTRL = (ADCB_CH2_CTRL & ~ADC_CH_INPUTMODE_gm) | mode;
			}
			
			// MARK: MUXCTRL
			inline static void
			setPositiveInput(adc::PositiveInput input)
			{
				ADCB_CH2_MUXCTRL = (ADCB_CH2_MUXCTRL & ~ADC_CH_MUXPOS_gm) | input;
			}
			
			inline static void
			setInternalInput(adc::InternalInput input)
			{
				setInputMode(adc::INPUTMODE_INTERNAL);
				ADCB_CH2_MUXCTRL = (ADCB_CH2_MUXCTRL & ~ADC_CH_MUXINT_gm) | input;
			}
			
			inline static void
			setNegativeInput(adc::NegativeInput input)
			{
				setInputMode(adc::INPUTMODE_DIFFERENTIAL);
				ADCB_CH2_MUXCTRL = (ADCB_CH2_MUXCTRL & ~ADC_CH_MUXNEG_gm) | input;
			}
			
			inline static void
			setNegativeInput(adc::NegativeInputWithGain input)
			{
				setInputMode(adc::INPUTMODE_DIFFERENTIAL_WITH_GAIN);
				ADCB_CH2_MUXCTRL = (ADCB_CH2_MUXCTRL & ~ADC_CH_MUXNEG_gm) | input;
			}
			
			// convenience methods
			inline static void
			setSingleEndedInput(adc::PositiveInput input)
			{
				setInputMode(adc::INPUTMODE_SINGLEENDED);
				setPositiveInput(input);
			}
			
			inline static void
			setDifferentialInput(adc::PositiveInput positiveInput, adc::NegativeInput negativeInput)
			{
				setPositiveInput(positiveInput);
				setNegativeInput(negativeInput);
			}
			
			inline static void
			setDifferentialInput(adc::PositiveInput positiveInput, adc::NegativeInputWithGain negativeInput, adc::Gain gain=adc::GAIN_1)
			{
				setGainFactor(gain);
				setPositiveInput(positiveInput);
				setNegativeInput(negativeInput);
			}
			
			// MARK: INTCTRL
			inline static void
			setInterruptMode(adc::InterruptMode mode=adc::INTERRUPT_ON_COMPLETE)
			{
				ADCB_CH2_INTCTRL = (ADCB_CH2_INTCTRL & ~ADC_CH_INTMODE_gm) | mode;
			}
			
			inline static void
			setInterruptLevel(InterruptLevel level=INTERRUPT_LEVEL_OFF)
			{
				ADCB_CH2_INTCTRL = (ADCB_CH2_INTCTRL & ~ADC_CH_INTLVL_gm) | (level << 0);
			}
			
			// MARK: INTFLAGS
			/// \return \c true if the conversion finished.
			inline static bool
			isConversionComplete()
			{
				return ADCB_CH2_INTFLAGS & ADC_CH_CHIF_bm;
			}
			
			// MARK: RES
			/// Read the last converted analog value
			inline static uint16_t
			getResult()
			{
				return ADCB_CH2_RES;
			}
			
			inline static ADC_CH_t&
			getChannelRegister()
			{
				return ADCB.CH2;
			}
			
			/**
			 * \brief	Read the value off the analog channel (convenience function)
			 * 
			 * Blocks until the conversion is done.
			 * 
			 * Equivalent to:
			 * \code
			 * startSingleConversion();
			 * while (!isConversionComplete())
			 *     ;
			 * getResult();
			 * \endcode
			 */
			static uint16_t
			read()
			{
				startSingleConversion();
				while(!isConversionComplete())
					;
				return getResult();
			}
		};
	}
}

#endif // ADCB
#endif // XPCC_ATXMEGA__ADCB_CHANNEL2_HPP
