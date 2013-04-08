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

#include "../gpio.hpp"
#include "../device.h"

#include "adc_2.hpp"

#include <xpcc_config.hpp>
#include <xpcc/architecture/driver/delay.hpp>	// xpcc::delay_us

using namespace xpcc::stm32;


#if defined(STM32F4XX) || defined(STM32F2XX) || defined(STM32F10X)

// ----------------------------------------------------------------------------
void
Adc2::setChannel(const Channels channel, const SampleTime sampleTime)
{
	ADC2->SQR1 = 0;		// clear number of conversions in the sequence and set number of conversions to 1
	ADC2->SQR3 = channel & 0b11111;

	if (static_cast<uint8_t>(channel) < 10) {
		ADC2->SMPR2 |= sampleTime << (static_cast<uint8_t>(channel) * 3);
	}
	else {
		ADC2->SMPR1 |= sampleTime << ((static_cast<uint8_t>(channel)-10) * 3);
	}
	/*
	 * FIXME: Setting Pins to analog mode based on the channel isn't really
	 * a great idea as long as we do not take package size into consideration.
	 * To quote STM32F302_3xx datasheet page 40:
	 * "the GPIO pins which are not present on these packages, must
	 *  not be configured in analog mode."
	 */
#if defined(STM32F2XX) || defined(STM32F4XX) 
	if (channel < 8)
	{
		GPIOA->MODER |= 0b11 << ((channel + 0) * 2);
	}
	else if (channel < 10)
	{
		GPIOB->MODER |= 0b11 << ((channel - 8) * 2);
	}
	else if (channel < 16)
	{
		GPIOC->MODER |= 0b11 << ((channel - 10) * 2);
	}
#elif defined(STM32F10X)
	if(channel < 8)
	{
		GPIOA->CRL &= ~(0b1111 << ((channel + 0) * 4));
	}
	else if (channel < 10)
	{
		GPIOB->CRL &= ~(0b1111 << ((channel - 8) * 4));
	}
	else if (channel < 16)
	{
		GPIOC->CRL &= ~(0b1111 << ((channel - 10) * 4));
	}
#endif
}

// ----------------------------------------------------------------------------
void
Adc2::disableInterrupt(const Interrupt interrupt)
{
#if defined(STM32F2XX) || defined(STM32F4XX)
	NVIC_DisableIRQ(ADC_IRQn);
#elif defined(STM32F10X)
	NVIC_DisableIRQ(ADC1_2_IRQn);
	#endif

	switch(interrupt)
	{
		case END_OF_CONVERSION_REGULAR:
			ADC2->CR1 &= ~ADC_CR1_EOCIE;
			break;
		case END_OF_CONVERSION_INJECTED:
			ADC2->CR1 &= ~ADC_CR1_JEOCIE;
			break;
		case ANALOG_WATCHDOG:
			ADC2->CR1 &= ~ADC_CR1_AWDIE;
			break;
#if defined(STM32F2XX) || defined(STM32F4XX)
		case OVERRUN:
			ADC2->CR1 &= ~ADC_CR1_OVRIE;
			break;
#endif
	}
}

// ----------------------------------------------------------------------------
void
Adc2::enableInterrupt(const Interrupt interrupt, const uint32_t priority)
{
#if defined(STM32F2XX) || defined(STM32F4XX)
	// Set priority for the interrupt vector				
	NVIC_SetPriority(ADC_IRQn, priority);				
	// register IRQ at the NVIC
	NVIC_EnableIRQ(ADC_IRQn);
#elif defined(STM32F10X)
	// Set priority for the interrupt vector
	NVIC_SetPriority(ADC1_2_IRQn, priority);
	// register IRQ at the NVIC
	NVIC_EnableIRQ(ADC1_2_IRQn);
	
#endif
	
	switch(interrupt)
	{
		case END_OF_CONVERSION_REGULAR:
			ADC2->CR1 |= ADC_CR1_EOCIE;
			break;
		case END_OF_CONVERSION_INJECTED:
			ADC2->CR1 |= ADC_CR1_JEOCIE;
			break;
		case ANALOG_WATCHDOG:
			ADC2->CR1 |= ADC_CR1_AWDIE;
			break;
#if defined(STM32F2XX) || defined(STM32F4XX)
		case OVERRUN:
			ADC2->CR1 |= ADC_CR1_OVRIE;
			break;
#endif
	}
}

#elif defined(STM32F3XX)
/* id < 4 */


void
Adc2::initialize(const ClockMode clk, const Prescaler pre,
		const CalibrationMode cal, const bool blocking)
{
	uint32_t tmp = 0;

	// enable clock
	RCC->AHBENR |= RCC_AHBENR_ADC12EN;

	// reset ADC
	// FIXME: not a good idea since you can only reset both
	// ADC1/ADC2 or ADC3/ADC4 at once ....
	// RCC->APB2RSTR |=  RCC_APB2RSTR_ADC2RST;
	// RCC->APB2RSTR &= ~RCC_APB2RSTR_ADC2RST;

	// set ADC "analog" clock source
	if (clk != ClockMode::DoNotChange) {
		if (clk == ClockMode::Asynchronous) {
			setPrescaler(pre);
		}
		tmp  =  ADC1_2->CCR;
		tmp &= ~ADC12_CCR_CKMODE;
		tmp |=  static_cast<uint32_t>(clk);
		ADC1_2->CCR = tmp;
	}

	// enable regulator
	ADC2->CR &= ~ADC_CR_ADVREGEN;
	ADC2->CR |= static_cast<uint32_t>(VoltageRegulatorState::Enabled);
	xpcc::delay_us(10);	// FIXME: this is ugly -> find better solution

	calibrate(cal, true);	// blocking calibration

	ADC2->CR |= ADC_CR_ADEN;
	if (blocking) {
		while(!isReady());
		resetInterruptFlags(InterruptFlag::Ready);
	}
}

#endif