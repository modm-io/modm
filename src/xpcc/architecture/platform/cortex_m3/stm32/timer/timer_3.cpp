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

#include "timer_3.hpp"

#include <xpcc_config.hpp>


// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer3::enable()
{
	// enable clock
	RCC->APB1ENR  |=  RCC_APB1ENR_TIM3EN;
	
	// reset timer
	RCC->APB1RSTR |=  RCC_APB1RSTR_TIM3RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;
}

void
xpcc::stm32::Timer3::disable()
{
	// disable clock
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM3EN;
	
	TIM3->CR1 = 0;
	TIM3->DIER = 0;
	TIM3->CCER = 0;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer3::setMode(Mode mode, SlaveMode slaveMode,
		SlaveModeTrigger slaveModeTrigger)
{
	// disable timer
	TIM3->CR1 = 0;
	TIM3->CR2 = 0;
	
	if (slaveMode == SLAVE_ENCODER_1 || \
		slaveMode == SLAVE_ENCODER_2 || \
		slaveMode == SLAVE_ENCODER_3)
	{
		// Prescaler has to be 1 when using the quadrature decoder
		setPrescaler(1);
	}
	
	// ARR Register is buffered, only Under/Overflow generates update interrupt
	TIM3->CR1 = TIM_CR1_ARPE | TIM_CR1_URS | mode;
	TIM3->SMCR = slaveMode | slaveModeTrigger;
}

// ----------------------------------------------------------------------------
xpcc::stm32::Timer3::Value
xpcc::stm32::Timer3::setPeriod(uint32_t microseconds, bool autoApply)
{
	// This will be inaccurate for non-smooth frequencies (last six digits
	// unequal to zero)
	uint32_t cycles = microseconds * (
			((STM32_APB1_FREQUENCY == STM32_AHB_FREQUENCY) ? 1 : 2) * 
					STM32_APB1_FREQUENCY / 1000000UL);
	uint16_t prescaler = (cycles + 65535) / 65536;	// always round up
	Value overflow = cycles / prescaler;
	
	overflow = overflow - 1;	// e.g. 36000 cycles are from 0 to 35999
	
	setPrescaler(prescaler);
	setOverflow(overflow);
	
	if (autoApply) {
		// Generate Update Event to apply the new settings for ARR
		TIM3->EGR |= TIM_EGR_UG;
	}
	
	return overflow;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer3::configureInputChannel(uint32_t channel,
		InputCaptureMapping input, InputCapturePrescaler prescaler,
		InputCapturePolarity polarity, uint8_t filter)
{
	channel -= 1;	// 1..4 -> 0..3
	
	// disable channel
	TIM3->CCER &= ~((TIM_CCER_CC1NP | TIM_CCER_CC1P | TIM_CCER_CC1E) << (channel * 4));
	
	uint32_t flags = input;
	flags |= ((uint32_t)prescaler) << 2;
	flags |= ((uint32_t)(filter&0xf)) << 4;
	
	if (channel <= 1)
	{
		uint32_t offset = 8 * channel;
		
		flags <<= offset;
		flags |= TIM3->CCMR1 & ~(0xff << offset);
		
		TIM3->CCMR1 = flags;
	}
	else {
		uint32_t offset = 8 * (channel - 2);
		
		flags <<= offset;
		flags |= TIM3->CCMR2 & ~(0xff << offset);
		
		TIM3->CCMR2 = flags; 
	}
	
	TIM3->CCER |= (TIM_CCER_CC1E | polarity) << (channel * 4);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer3::configureOutputChannel(uint32_t channel,
		OutputCompareMode mode, Value compareValue)
{
	channel -= 1;	// 1..4 -> 0..3
	
	// disable channel
	TIM3->CCER &= ~((TIM_CCER_CC1NP | TIM_CCER_CC1P | TIM_CCER_CC1E) << (channel * 4));
	
	setCompareValue(channel + 1, compareValue);
	
	// enable preload (the compare value is loaded at each update event)
	uint32_t flags = mode | TIM_CCMR1_OC1PE;
	
	if (channel <= 1)
	{
		uint32_t offset = 8 * channel;
		
		flags <<= offset;
		flags |= TIM3->CCMR1 & ~(0xff << offset);
		
		TIM3->CCMR1 = flags;
	}
	else {
		uint32_t offset = 8 * (channel - 2);
		
		flags <<= offset;
		flags |= TIM3->CCMR2 & ~(0xff << offset);
		
		TIM3->CCMR2 = flags; 
	}
	
	if (mode != OUTPUT_INACTIVE) {
		TIM3->CCER |= (TIM_CCER_CC1E) << (channel * 4);
	}
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer3::enableInterruptVector(bool enable, uint32_t priority)
{
	if (enable) {
		// Set priority for the interrupt vector
		NVIC_SetPriority(TIM3_IRQn, priority);
		
		// register IRQ at the NVIC
		NVIC_EnableIRQ(TIM3_IRQn);
	}
	else {
		NVIC_DisableIRQ(TIM3_IRQn);
	}
}

