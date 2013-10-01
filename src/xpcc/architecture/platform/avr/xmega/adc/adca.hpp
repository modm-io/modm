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

#ifndef XPCC_ATXMEGA__ADC_A_HPP
#define XPCC_ATXMEGA__ADC_A_HPP

#include <avr/io.h>
#include <stdint.h>
#include <xpcc/architecture/driver/atomic/lock.hpp>
#include "adc.hpp"

#if defined(ADCA) || defined(__DOXYGEN__)

namespace xpcc
{
	namespace xmega
	{
		/**
		 * \brief		ADC Module A
		 * 
		 * Each ADC module has four channels with its own MUX selection and result
		 * register. See AdcChannelA0..3.
		 * 
		 * \ingroup		xmega_adc
		 */
		class AdcA
		{
		public:
			/**
			 * \brief	Initialize ADC module
			 * 
			 * Enables the module and loads the ADC calibration registers. Default
			 * is right adjusted 12-bit unsigned conversion mode.
			 */
			static void
			initialize(adc::Reference reference=adc::REFERENCE_INTERNAL_VCC,
					   adc::Prescaler prescaler=adc::PRESCALER_512,
					   adc::Resolution resolution=adc::RESOLUTION_12BIT);
			
			// MARK: CTRLA
			inline static void
			setDmaRequest(adc::DmaRequest request=adc::DMA_REQUEST_OFF)
			{
				ADCA_CTRLA = (ADCA_CTRLA & ~ADC_DMASEL_gm) | request;
			}
			
			inline static void
			startSingleConversion(adc::Channel channel)
			{
				ADCA_CTRLA |= (channel << 2);
			}
			
			/**
			 * \brief	Flush the ADC pipeline.
			 * 
			 * Starting an ADC conversion may cause an unknown delay between the
			 * software start or event and the actual conversion start since
			 * conversion of other higher priority ADC channels may be pending, or
			 * since the System clock may be much faster than the ADC Clock.
			 * 
			 * To start an ADC conversion immediately on an incoming event, it is
			 * possible to flush the ADC for all measurements, reset the ADC clock
			 * and start the conversion at the next Peripheral clock cycle, which
			 * then will also be the next ADC clock cycle.
			 * 
			 * If this is done all ongoing conversions in the ADC pipeline will be
			 * lost.
			 */
			inline static void
			flush()
			{
				ADCA_CTRLA |= ADC_FLUSH_bm;
			}
			
			inline static void
			enable(bool enable=true)
			{
				ADCA_CTRLA = (ADCA_CTRLA & ~ADC_ENABLE_bm) | (enable ? ADC_ENABLE_bm : 0);
			}
			
			// MARK: CTRLB
			/**
			 * \brief	Set signed or unsigned conversion.
			 * 
			 * Changing the mode to signed conversion will require the setup of two pins.
			 */
			inline static void
			setSignedConversion(bool signedMode=true)
			{
				ADCA_CTRLB = (ADCA_CTRLB & ~ADC_CONMODE_bm) | (signedMode ? ADC_CONMODE_bm : 0);
			}
			
			/**
			 * \brief	Enable free running mode as defined in setChannelSweep.
			 *
			 * \see		setChannelSweep()
			 */
			inline static void
			setFreeRunningMode(bool enable=true)
			{
				ADCA_CTRLB = (ADCA_CTRLB & ~ADC_FREERUN_bm) | (enable ? ADC_FREERUN_bm : 0);
			}
			
			/**
			 * \brief	left/right-adjusted 12-bit result or 8-bit result
			 * 
			 * Default is right adjusted 12-bit mode.
			 */
			inline static void
			setResolution(adc::Resolution resolution=adc::RESOLUTION_12BIT)
			{
				ADCA_CTRLB = (ADCA_CTRLB & ~ADC_RESOLUTION_gm) | resolution;
			}
			
			// MARK: REFCTRL
			/**
			 * \brief	Set reference voltage
			 * 
			 * The ADC has four possible reference voltages:
			 * - Internal 1V (\c REFERENCE_INTERNAL_1V)
			 * - Vcc/1.6 (\c REFERENCE_INTERNAL_VCC)
			 * - AREF on PORTA (\c REFERENCE_EXTERNAL_PORTA)
			 * - AREF on PORTB (\c REFERENCE_EXTERNAL_PORTB)
			 * - AVCC/2 (only on Xmega D devices)
			 * 
			 * The voltage on the AREF pins is limited to 1V < Vref < Vcc - 0.6V.
			 * For a 3.3V supply voltage this puts the upper limit on Vref to 2.7V.
			 * 
			 * In unsigned mode a small offset of 0.05*Vref is subtracted. Therefore
			 * the measurement of AGND will lead to value of about 200. This also
			 * limits the input range from AGND to 0.95*Vref.
			 */
			inline static void
			setReference(adc::Reference reference=adc::REFERENCE_INTERNAL_VCC)
			{
				ADCA_REFCTRL = reference;
			}
			
			// MARK: EVCTRL
			/**
			 * \brief	Configure channel sweep
			 * 
			 * Select which channels to include in free-running mode. You can
			 * choose between channel 0 only, channel 0 and 1, channel 0 to 2 or
			 * all four channels.
			 * 
			 * Care should be taken not to change any involved MUX settings when
			 * in free-running mode, as this would corrupt conversion results.
			 * 
			 * \see		setFreeRunningMode()
			 */
			inline static void
			setChannelSweep(adc::ChannelSweep sweep)
			{
				ADCA_EVCTRL = (ADCA_EVCTRL & ~ADC_SWEEP_gm) | sweep;
			}
			
			inline static void
			setEventChannel(adc::EventChannel channel)
			{
				ADCA_EVCTRL = (ADCA_EVCTRL & ~ADC_EVSEL_gm) | channel;
			}
			
			inline static void
			setEventAction(adc::EventAction action=adc::EVENT_ACTION_NONE)
			{
				ADCA_EVCTRL = (ADCA_EVCTRL & ~ADC_EVACT_gm) | action;
			}
			
			// MARK: PRESCALER
			/**
			 * \brief	Set prescaler
			 * 
			 * ADC clock frequency should be at around 62kHz for maximum
			 * resolution of internal sources.
			 * 
			 * For external measurements the frequency should be between 100kHz
			 * and 2MHz.
			 */
			inline static void
			setPrescaler(adc::Prescaler prescaler=adc::PRESCALER_512)
			{
				ADCA_PRESCALER = prescaler;
			}
			
			// MARK: INTFLAGS
			inline static uint8_t
			getInterruptFlags(bool clear=false)
			{
				uint8_t flags = ADCA_INTFLAGS & 0x0f;
				if (clear) ADCA_INTFLAGS = flags;
				return flags;
			}
			
			// MARK: CMP
			inline static void
			setCompareValue(uint16_t value)
			{
				xpcc::atomic::Lock lock;
				ADCA_CMP = value;
			}
			
			inline static ADC_t&
			getModuleRegister()
			{
				return ADCA;
			}
		};
	}
}

#endif // ADCA
#endif // XPCC_ATXMEGA__ADC_A_HPP
