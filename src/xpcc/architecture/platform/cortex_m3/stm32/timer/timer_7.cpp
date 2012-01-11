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

#include "timer_7.hpp"

#if defined(STM32F10X_HD) || defined(STM32F10X_XL) || \
	defined(STM32F10X_CL) || \
	defined(STM32F2XX) || defined(STM32F4XX)

#include <xpcc_config.hpp>

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer7::enable()
{
	// enable clock
	RCC->APB1ENR  |=  RCC_APB1ENR_TIM7EN;
	
	// reset timer
	RCC->APB1RSTR |=  RCC_APB1RSTR_TIM7RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM7RST;
}

void
xpcc::stm32::Timer7::disable()
{
	// disable clock
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM7EN;
	
	TIM7->CR1 = 0;
	TIM7->DIER = 0;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer7::setMode(Mode mode)
{
	// ARR Register is buffered, only Under/Overflow generates update interrupt
	TIM7->CR1 = TIM_CR1_ARPE | TIM_CR1_URS | mode;
	TIM7->CR2 = 0;
}

// ----------------------------------------------------------------------------
uint16_t
xpcc::stm32::Timer7::setPeriod(uint32_t microseconds, bool autoApply)
{
	// This will be inaccurate for non-smooth frequencies (last six digits
	// unequal to zero)
#if defined(STM32F2XX)
#warning "Check if these values are correct!"
	uint32_t cycles = microseconds * 60;	// APB1 clock * 2 in MHz = 30 * 2 = 60
#elif defined(STM32F4XX)
	uint32_t cycles = microseconds * 84;	// APB1 clock * 2 in MHz = 42 * 2 = 84
#else
	uint32_t cycles = microseconds * (
		((STM32_APB1_FREQUENCY == STM32_AHB_FREQUENCY) ? 1 : 2) * 
			STM32_APB1_FREQUENCY / 1000000UL);
#endif
	
	uint16_t prescaler = (cycles + 65535) / 65536;	// always round up
	uint16_t overflow = cycles / prescaler;
	
	overflow = overflow - 1;	// e.g. 36000 cycles are from 0 to 35999
	
	setPrescaler(prescaler);
	setOverflow(overflow);
	
	if (autoApply) {
		// Generate Update Event to apply the new settings for ARR
		TIM7->EGR |= TIM_EGR_UG;
	}
	
	return overflow;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer7::enableInterruptVector(bool enable, uint32_t priority)
{
	if (enable)
	{
		// Set vector priority
		NVIC_SetPriority(TIM7_IRQn, priority);
		
		// register IRQ at the NVIC
		NVIC_EnableIRQ(TIM7_IRQn);
	}
	else {
		NVIC_DisableIRQ(TIM7_IRQn);
	}
}

#endif