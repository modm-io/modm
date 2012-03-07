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

#include <avr/interrupt.h>
#include "adc_a_channel_1.hpp"

#ifdef ADCA

// ----------------------------------------------------------------------------
uint16_t
xpcc::atxmega::AdcChannelA1::read()
{
	startConversion();
	
	while(!isFinished()) {
		// wait until the conversion is finished
	}
	
	return getResult();
}

void
xpcc::atxmega::AdcChannelA1::setInternalInputMode(ADC_CH_MUXINT_t input)
{
	ADCA_CH1_CTRL = ADC_CH_INPUTMODE_INTERNAL_gc;
	ADCA_CH1_MUXCTRL = input;
}

void
xpcc::atxmega::AdcChannelA1::setSingleEndedMode(ADC_CH_MUXPOS_t input)
{
	ADCA_CH1_CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA_CH1_MUXCTRL = input;
}

void
xpcc::atxmega::AdcChannelA1::setDifferentialMode(ADC_CH_MUXPOS_t positiveInput, ADC_CH_MUXNEG_t negativeInput)
{
	setSignedConversion(true);
	
	ADCA_CH1_CTRL = ADC_CH_INPUTMODE_DIFF_gc;
	ADCA_CH1_MUXCTRL = positiveInput | negativeInput;
}

void
xpcc::atxmega::AdcChannelA1::setDifferentialGainMode(
		ADC_CH_MUXPOS_t positiveInput, ADC_CH_MUXNEG_t negativeInput,
		ADC_CH_GAIN_t gainFactor)
{
	setSignedConversion(true);
	
	ADCA_CH1_CTRL = ADC_CH_INPUTMODE_DIFFWGAIN_gc | gainFactor;
	ADCA_CH1_MUXCTRL = positiveInput | negativeInput;
}

#endif	// ADCA