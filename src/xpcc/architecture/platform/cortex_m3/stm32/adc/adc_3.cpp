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

#include "adc_3.hpp"


#if defined(STM32F2XX) || defined(STM32F4XX) || \
	defined (STM32F10X_HD) || defined (STM32F10X_XL)
using namespace xpcc::stm32;

// ----------------------------------------------------------------------------
void
Adc3::setChannel(const Channels channel, const SampleTime sampleTime)
{
	ADC3->SQR1 = 0;		// clear number of conversions in the sequence and set number of conversions to 1
	ADC3->SQR3 = channel & 0b11111;

	if (static_cast<uint8_t>(channel) < 10) {
		ADC3->SMPR2 |= sampleTime << (static_cast<uint8_t>(channel) * 3);
	}
	else {
		ADC3->SMPR1 |= sampleTime << ((static_cast<uint8_t>(channel)-10) * 3);
	}

#if defined(STM32F2XX) || defined(STM32F4XX) 
	if(channel < 4)
	{
		GPIOA->MODER |= 0b11 << ((channel + 0) * 2);
	}
	else if (channel < 9)
	{
		GPIOF->MODER |= 0b11 << ((channel + 2) * 2);
	}
	else if (channel == 9)
	{
		GPIOF->MODER |= 0b11 << ((channel - 6) * 2);
	}
	else if (channel < 14)
	{
		GPIOC->MODER |= 0b11 << ((channel - 10) * 2);
	}
	else if (channel < 16)
	{
		GPIOF->MODER |= 0b11 << ((channel - 10) * 2);
	}
#elif defined(STM32F10X)
	if(channel < 4)
	{
		GPIOA->CRL &= ~(0b1111 << ((channel + 0) * 4));
	}
	else if (channel < 6)
	{
		GPIOF->CRL &= ~(0b1111 << ((channel + 2) * 4));
	}
	else if (channel < 9)
	{
		GPIOF->CRH &= ~(0b1111 << ((channel - 6) * 4));
	}
	else if (channel < 14 && channel > 9)
	{
		GPIOC->CRL &= ~(0b1111 << ((channel - 10) * 4));
	}
#endif
}

// ----------------------------------------------------------------------------
void
Adc3::disableInterrupt(const Interrupt interrupt)
{
#if defined(STM32F2XX) || defined(STM32F4XX)
	NVIC_DisableIRQ(ADC_IRQn);
#elif defined(STM32F10X)
	NVIC_DisableIRQ(ADC3_IRQn);
	#endif

	switch(interrupt)
	{
		case END_OF_CONVERSION_REGULAR:
			ADC3->CR1 &= ~ADC_CR1_EOCIE;
			break;
		case END_OF_CONVERSION_INJECTED:
			ADC3->CR1 &= ~ADC_CR1_JEOCIE;
			break;
		case ANALOG_WATCHDOG:
			ADC3->CR1 &= ~ADC_CR1_AWDIE;
			break;
#if defined(STM32F2XX) || defined(STM32F4XX)
		case OVERRUN:
			ADC3->CR1 &= ~ADC_CR1_OVRIE;
			break;
#endif
	}
}

// ----------------------------------------------------------------------------
void
Adc3::enableInterrupt(const Interrupt interrupt, const uint32_t priority)
{
#if defined(STM32F2XX) || defined(STM32F4XX)
	// Set priority for the interrupt vector				
	NVIC_SetPriority(ADC_IRQn, priority);				
	// register IRQ at the NVIC
	NVIC_EnableIRQ(ADC_IRQn);
#elif defined(STM32F10X)
	// Set priority for the interrupt vector
	NVIC_SetPriority(ADC3_IRQn, priority);
	// register IRQ at the NVIC
	NVIC_EnableIRQ(ADC3_IRQn);
#endif
	
	switch(interrupt)
	{
		case END_OF_CONVERSION_REGULAR:
			ADC3->CR1 |= ADC_CR1_EOCIE;
			break;
		case END_OF_CONVERSION_INJECTED:
			ADC3->CR1 |= ADC_CR1_JEOCIE;
			break;
		case ANALOG_WATCHDOG:
			ADC3->CR1 |= ADC_CR1_AWDIE;
			break;
#if defined(STM32F2XX) || defined(STM32F4XX)
		case OVERRUN:
			ADC3->CR1 |= ADC_CR1_OVRIE;
			break;
#endif
	}
}


#endif	// defined(STM32F2XX) || defined(STM32F4XX) || defined (STM32F10X_HD) || defined (STM32F10X_XL)
