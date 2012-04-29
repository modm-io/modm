// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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

#ifndef XPCC_ATXMEGA__ADC_HPP
#define	XPCC_ATXMEGA__ADC_HPP

namespace xpcc
{
	namespace atxmega
	{
		/**
		 * \brief Configuration options for the ADC.
		 *
		 * \author	Niklas Hauser
		 * \ingroup atxmega_adc
		 */
		namespace adc
		{
			enum Channel
			{
				CHANNEL0 = 0x01,
				CHANNEL1 = 0x02,
				CHANNEL2 = 0x04,
				CHANNEL3 = 0x08
			};
			
			// MARK: ADC Module
			// MARK: CTRLA
			enum DmaRequest
			{
				DMA_REQUEST_OFF = ADC_DMASEL_OFF_gc,		///< Combined DMA request OFF
				DMA_REQUEST_CH01 = ADC_DMASEL_CH01_gc,		///< ADC Channel 0 or 1
				DMA_REQUEST_CH012 = ADC_DMASEL_CH012_gc,	///< ADC Channel 0 or 1 or 2
				DMA_REQUEST_CH0123 = ADC_DMASEL_CH0123_gc,	///< ADC Channel 0 or 1 or 2 or 3
			};
			
			// MARK: CTRLB
			enum Resolution
			{
				RESOLUTION_12BIT = ADC_RESOLUTION_12BIT_gc,	///< 12-bit right-adjusted result
				RESOLUTION_8BIT = ADC_RESOLUTION_8BIT_gc,	///< 8-bit right-adjusted result
				RESOLUTION_12BIT_LEFT_ADJUSTED = ADC_RESOLUTION_LEFT12BIT_gc,	///< 12-bit left-adjusted result
			};
			
			// MARK: REFCTRL
			enum Reference
			{
				REFERENCE_INTERNAL_1V = ADC_REFSEL_INT1V_gc,	///< Internal 1V
				REFERENCE_INTERNAL_VCC = ADC_REFSEL_VCC_gc,		///< Internal VCC / 1.6V
				REFERENCE_EXTERNAL_PORTA = ADC_REFSEL_AREFA_gc,	///< External reference on PORT A
				REFERENCE_EXTERNAL_PORTB = ADC_REFSEL_AREFB_gc,	///< External reference on PORT B
				REFERENCE_INTERNAL_TEMPERATURE = ADC_TEMPREF_bm,	///< Internal temperature reference
			};
			
			// MARK: EVCTRL
			enum ChannelSweep
			{
				SWEEP_0 = ADC_SWEEP_0_gc,		///< ADC Channel 0
				SWEEP_01 = ADC_SWEEP_01_gc,		///< ADC Channel 0,1
				SWEEP_012 = ADC_SWEEP_012_gc,	///< ADC Channel 0,1,2
				SWEEP_0123 = ADC_SWEEP_0123_gc,	///< ADC Channel 0,1,2,3
			};
			
			enum EventChannel
			{
				EVENT_CHANNEL_0123 = ADC_EVSEL_0123_gc,	///< Event Channel 0,1,2,3
				EVENT_CHANNEL_1234 = ADC_EVSEL_1234_gc,	///< Event Channel 1,2,3,4
				EVENT_CHANNEL_2345 = ADC_EVSEL_2345_gc,	///< Event Channel 2,3,4,5
				EVENT_CHANNEL_3456 = ADC_EVSEL_3456_gc,	///< Event Channel 3,4,5,6
				EVENT_CHANNEL_4567 = ADC_EVSEL_4567_gc,	///< Event Channel 4,5,6,7
				EVENT_CHANNEL_567 = ADC_EVSEL_567_gc,	///< Event Channel 5,6,7
				EVENT_CHANNEL_67 = ADC_EVSEL_67_gc,		///< Event Channel 6,7
				EVENT_CHANNEL_7 = ADC_EVSEL_7_gc,		///< Event Channel 7
			};
			
			enum EventAction
			{
				EVENT_ACTION_NONE = ADC_EVACT_NONE_gc,	///< No event action
				EVENT_ACTION_CH0 = ADC_EVACT_CH0_gc,	///< First event triggers channel 0
				EVENT_ACTION_CH01 = ADC_EVACT_CH01_gc,	///< First two events trigger channel 0,1
				EVENT_ACTION_CH012 = ADC_EVACT_CH012_gc,	///< First three events trigger channel 0,1,2
				EVENT_ACTION_CH0123 = ADC_EVACT_CH0123_gc,	///< Events trigger channel 0,1,2,3
				EVENT_ACTION_SWEEP = ADC_EVACT_SWEEP_gc,	///< First event triggers sweep
				EVENT_ACTION_SYNCHRONIZED_SWEEP = ADC_EVACT_SYNCHSWEEP_gc,	///< First event triggers synchronized sweep
			};
			
			// MARK: PRESCALER
			enum Prescaler
			{
				PRESCALER_4 = ADC_PRESCALER_DIV4_gc,
				PRESCALER_8 = ADC_PRESCALER_DIV8_gc,
				PRESCALER_16 = ADC_PRESCALER_DIV16_gc,
				PRESCALER_32 = ADC_PRESCALER_DIV32_gc,
				PRESCALER_64 = ADC_PRESCALER_DIV64_gc,
				PRESCALER_128 = ADC_PRESCALER_DIV128_gc,
				PRESCALER_256 = ADC_PRESCALER_DIV256_gc,
				PRESCALER_512 = ADC_PRESCALER_DIV512_gc,
			};
			
			// MARK: INTFLAGS
			enum InterruptFlags
			{
				INTERRUPT_FLAG_CH0 = 0x01,
				INTERRUPT_FLAG_CH1 = 0x02,
				INTERRUPT_FLAG_CH2 = 0x04,
				INTERRUPT_FLAG_CH3 = 0x08
			};
			
			
			// MARK: ADC Channel Module
			// MARK: CTRL
			enum Gain
			{
				GAIN_1 = ADC_CH_GAIN_1X_gc,	///< 1x gain
				GAIN_2 = ADC_CH_GAIN_2X_gc,	///< 2x gain
				GAIN_4 = ADC_CH_GAIN_4X_gc,	///< 4x gain
				GAIN_8 = ADC_CH_GAIN_8X_gc,	///< 8x gain
				GAIN_16 = ADC_CH_GAIN_16X_gc,	///< 16x gain
				GAIN_32 = ADC_CH_GAIN_32X_gc,	///< 32x gain
				GAIN_64 = ADC_CH_GAIN_64X_gc,	///< 64x gain
			};
			
			enum InputMode
			{
				INPUTMODE_INTERNAL = ADC_CH_INPUTMODE_INTERNAL_gc,	///< Internal inputs, no gain
				INPUTMODE_SINGLEENDED = ADC_CH_INPUTMODE_SINGLEENDED_gc,	///< Single-ended input, no gain
				INPUTMODE_DIFFERENTIAL = ADC_CH_INPUTMODE_DIFF_gc,	///< Differential input, no gain
				INPUTMODE_DIFFERENTIAL_WITH_GAIN = ADC_CH_INPUTMODE_DIFFWGAIN_gc,	///< Differential input, with gain
			};
			
			// MARK: MUXCTRL
			enum PositiveInput
			{
				POSITIVE_PIN0 = ADC_CH_MUXPOS_PIN0_gc,	///< Input pin 0
				POSITIVE_PIN1 = ADC_CH_MUXPOS_PIN1_gc,	///< Input pin 1
				POSITIVE_PIN2 = ADC_CH_MUXPOS_PIN2_gc,	///< Input pin 2
				POSITIVE_PIN3 = ADC_CH_MUXPOS_PIN3_gc,	///< Input pin 3
				POSITIVE_PIN4 = ADC_CH_MUXPOS_PIN4_gc,	///< Input pin 4
				POSITIVE_PIN5 = ADC_CH_MUXPOS_PIN5_gc,	///< Input pin 5
				POSITIVE_PIN6 = ADC_CH_MUXPOS_PIN6_gc,	///< Input pin 6
				POSITIVE_PIN7 = ADC_CH_MUXPOS_PIN7_gc,	///< Input pin 7
			};
			
			enum InternalInput
			{
				INTERNAL_TEMPERATURE = ADC_CH_MUXINT_TEMP_gc,	///< Temperature Reference
				INTERNAL_BANDGAP = ADC_CH_MUXINT_BANDGAP_gc,	///< Bandgap Reference
				INTERNAL_SCALED_VCC = ADC_CH_MUXINT_SCALEDVCC_gc,	///< 1/10 scaled VCC
				INTERNAL_DAC = ADC_CH_MUXINT_DAC_gc,	///< DAC output
			};
			
			enum NegativeInput
			{
				NEGATIVE_PIN0 = ADC_CH_MUXNEG_PIN0_gc,	///< Input pin 0
				NEGATIVE_PIN1 = ADC_CH_MUXNEG_PIN1_gc,	///< Input pin 1
				NEGATIVE_PIN2 = ADC_CH_MUXNEG_PIN2_gc,	///< Input pin 2
				NEGATIVE_PIN3 = ADC_CH_MUXNEG_PIN3_gc,	///< Input pin 3
			};
			
			enum NegativeInputWithGain
			{
				NEGATIVE_PIN4 = ADC_CH_MUXNEG_PIN4_gc,	///< Input pin 4
				NEGATIVE_PIN5 = ADC_CH_MUXNEG_PIN5_gc,	///< Input pin 5
				NEGATIVE_PIN6 = ADC_CH_MUXNEG_PIN6_gc,	///< Input pin 6
				NEGATIVE_PIN7 = ADC_CH_MUXNEG_PIN7_gc,	///< Input pin 7
			};
			
			enum InterruptMode
			{
				INTERRUPT_ON_COMPLETE = ADC_CH_INTMODE_COMPLETE_gc,	///< Interrupt on conversion complete
				INTERRUPT_ON_BELOW_COMPARE = ADC_CH_INTMODE_BELOW_gc,	///< Interrupt on result below compare value
				INTERRUPT_ON_ABOVE_COMPARE = ADC_CH_INTMODE_ABOVE_gc,	///< Interrupt on result above compare value
			};
		}
	}
}

#endif	// XPCC_ATXMEGA__ADC_HPP