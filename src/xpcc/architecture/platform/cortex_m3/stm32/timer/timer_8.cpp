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

#include "../gpio.hpp"
#include "../device.h"

#include "timer_8.hpp"

#include <xpcc_config.hpp>

#if defined (STM32F10X_HD) || defined (STM32F10X_XL) 


#if defined(STM32F10X_XL) || defined(STM32F2XX) || defined(STM32F4XX)
#	define TIM1_BRK_IRQn		TIM1_BRK_TIM9_IRQn
#	define TIM1_UP_IRQn			TIM1_UP_TIM10_IRQn
#	define TIM1_TRG_COM_IRQn	TIM1_TRG_COM_TIM11_IRQn

#	define TIM8_BRK_IRQn		TIM8_BRK_TIM12_IRQn
#	define TIM8_UP_IRQn			TIM8_UP_TIM13_IRQn
#	define TIM8_TRG_COM_IRQn	TIM8_TRG_COM_TIM14_IRQn
#endif

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer8::enable()
{
	// enable clock
	RCC->APB2ENR  |=  RCC_APB2ENR_TIM8EN;
	
	// reset timer
	RCC->APB2RSTR |=  RCC_APB2RSTR_TIM8RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_TIM8RST;
}

void
xpcc::stm32::Timer8::disable()
{
	// disable clock
	RCC->APB2ENR &= ~RCC_APB2ENR_TIM8EN;
	
	TIM8->CR1 = 0;
	TIM8->DIER = 0;
	TIM8->CCER = 0;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer8::setMode(Mode mode, SlaveMode slaveMode, SlaveModeTrigger slaveModeTrigger)
{
	// disable timer
	TIM8->CR1 = 0;
	TIM8->CR2 = 0;
	
	if (slaveMode == SLAVE_ENCODER_1 || slaveMode == SLAVE_ENCODER_2 || slaveMode == SLAVE_ENCODER_3)
	{
		setPrescaler(1);
	}
	
	// ARR Register is buffered, only Under/Overflow generates update interrupt
	TIM8->CR1 = TIM_CR1_ARPE | TIM_CR1_URS | mode;
	TIM8->SMCR = slaveMode|slaveModeTrigger;
}

// ----------------------------------------------------------------------------
uint16_t
xpcc::stm32::Timer8::setPeriod(uint32_t microseconds, bool autoApply)
{
	// This will be inaccurate for non-smooth frequencies (last six digits
	// unequal to zero)
	uint32_t cycles = microseconds * (
			((STM32_APB2_FREQUENCY==STM32_AHB_FREQUENCY)?1:2) * 
			STM32_APB2_FREQUENCY / 1000000UL);
	uint16_t prescaler = (cycles + 65535) / 65536;	// always round up
	uint16_t overflow = cycles / prescaler;
	
	overflow = overflow - 1;	// e.g. 36000 cycles are from 0 to 35999
	
	setPrescaler(prescaler);
	setOverflow(overflow);
	
	if (autoApply) {
		// Generate Update Event to apply the new settings for ARR
		TIM8->EGR |= TIM_EGR_UG;
	}
	
	return overflow;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer8::configureInputChannel(uint32_t channel,
		InputCaptureMapping input, InputCapturePrescaler prescaler, InputCapturePolarity polarity, uint8_t filter)
{
	channel -= 1;	// 1..4 -> 0..3
	
	// disable channel
	TIM8->CCER &= ~((TIM_CCER_CC1NP | TIM_CCER_CC1P | TIM_CCER_CC1E) << (channel * 4));
	
	uint32_t flags = input;
	flags |= ((uint32_t)prescaler) << 2;
	flags |= ((uint32_t)(filter&0xf)) << 4;
	
	if (channel <= 1)
	{
		uint32_t offset = 8 * channel;
		
		flags <<= offset;
		flags |= TIM8->CCMR1 & ~(0xff << offset);
		
		TIM8->CCMR1 = flags;
	}
	else {
		uint32_t offset = 8 * (channel - 2);
		
		flags <<= offset;
		flags |= TIM8->CCMR2 & ~(0xff << offset);
		
		TIM8->CCMR2 = flags; 
	}
	
	TIM8->CCER |= (TIM_CCER_CC1E | polarity) << (channel * 4);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer8::configureOutputChannel(uint32_t channel,
		OutputCompareMode mode, uint16_t compareValue)
{
	channel -= 1;	// 1..4 -> 0..3
	
	// disable output
	TIM8->CCER &= ~(0xf << (channel * 4));
	
	setCompareValue(channel + 1, compareValue);
	
	// enable preload (the compare value is loaded at each update event)
	uint32_t flags = mode | TIM_CCMR1_OC1PE;
		
	if (channel <= 1)
	{
		uint32_t offset = 8 * channel;
		
		flags <<= offset;
		flags |= TIM8->CCMR1 & ~(0xff << offset);
		
		TIM8->CCMR1 = flags;
	}
	else {
		uint32_t offset = 8 * (channel - 2);
		
		flags <<= offset;
		flags |= TIM8->CCMR2 & ~(0xff << offset);
		
		TIM8->CCMR2 = flags; 
	}
	
	// Disable Repetition Counter (FIXME has to be done here for some unknown reason)
	TIM8->RCR = 0;
	
	if (mode != OUTPUT_INACTIVE) {
		TIM8->CCER |= (TIM_CCER_CC1E) << (channel * 4);
	}
}

// ----------------------------------------------------------------------------
// Re-implemented here to save some code space. As all arguments in the calls
// below are constant the compiler is able to calculate everything at
// compile time.
static ALWAYS_INLINE void
nvicEnableInterrupt(IRQn_Type IRQn)
{
	NVIC->ISER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

static ALWAYS_INLINE void
nvicDisableInterrupt(IRQn_Type IRQn)
{
	NVIC_DisableIRQ(IRQn);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer8::setInterruptVectorEnabled(Interrupt interrupts)
{
	// register IRQs at the NVIC
	if (interrupts & (INTERRUPT_UPDATE)) {
		nvicEnableInterrupt(TIM8_UP_IRQn);
	}
	else{
		nvicDisableInterrupt(TIM8_UP_IRQn);
	}
	
	if (interrupts & INTERRUPT_BREAK) {
		nvicEnableInterrupt(TIM8_BRK_IRQn);
	}
	else{
		nvicDisableInterrupt(TIM8_BRK_IRQn);
	}
	
	if (interrupts & (INTERRUPT_COM | INTERRUPT_TRIGGER)) {
		nvicEnableInterrupt(TIM8_TRG_COM_IRQn);
	}	
	else{
		nvicDisableInterrupt(TIM8_TRG_COM_IRQn);
	}
	
	if (interrupts & 
			(INTERRUPT_CAPTURE_COMPARE_1 | INTERRUPT_CAPTURE_COMPARE_2 |
			INTERRUPT_CAPTURE_COMPARE_3 | INTERRUPT_CAPTURE_COMPARE_4)) {
		nvicEnableInterrupt(TIM8_CC_IRQn);
	}
	else{
		nvicDisableInterrupt(TIM8_CC_IRQn);
	}
	
}


#endif
