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

//#include <libmaple/usart.h>
#include "../gpio.hpp"
#include "../device.h"

#include "timer_5.hpp"

#if defined (STM32F10X_HD) || defined (STM32F10X_CL) || defined (STM32F10X_XL)

/*namespace
{
	GPIO__OUTPUT(Txd, C, 12);
	GPIO__INPUT(Rxd, D, 2);
	
	static const uint32_t nvicId = 53;
	static const uint32_t apbId = 20;
	static const uint32_t apbClk = 36000000;	// APB1
}*/

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer5::enable()
{
	// enable clock
	RCC->APB1ENR  |=  RCC_APB1ENR_TIM5EN;
	
	// reset timer
	RCC->APB1RSTR |=  RCC_APB1RSTR_TIM5RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM5RST;
	
	// register IRQ at the NVIC
	NVIC_EnableIRQ(TIM5_IRQn);
}

void
xpcc::stm32::Timer5::disable()
{
	// disable clock
	RCC->APB1ENR &= ~RCC_APB1ENR_TIM5EN;
	
	TIM5->CR1 = 0;
	TIM5->DIER = 0;
	TIM5->CCER = 0;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer5::configure()
{
	// ARR Register is buffered, only Under/Overflow generates update interrupt
	TIM3->CR1 = TIM_CR1_ARPE | TIM_CR1_URS;
	TIM3->CR2 = 0;

	// Slave mode disabled
	TIM3->SMCR = 0;
}

// ----------------------------------------------------------------------------
uint16_t
xpcc::stm32::Timer5::setPeriod(uint32_t microseconds, bool autoRefresh)
{
	// This will be inaccurate for non-smooth frequencies (last six digits
	// unequal to zero)
	uint32_t cycles = microseconds * (F_CPU / 1000000UL);
	uint16_t prescaler = (cycles + 65535) / 65536;	// always round up
	uint16_t overflow = cycles / prescaler;
	
	overflow = overflow - 1;	// e.g. 36000 cycles are from 0 to 35999
	
	setPrescaler(prescaler);
	setOverflow(overflow);
	
	if (autoRefresh) {
		refresh();
	}
	
	return overflow;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Timer5::enableInterrupt(Interrupt interrupt)
{
	TIM5->DIER |= interrupt;
}

void
xpcc::stm32::Timer5::disableInterrupt(Interrupt interrupt)
{
	TIM5->DIER &= ~interrupt;
}

void
xpcc::stm32::Timer5::acknowledgeInterrupt(Interrupt interrupt)
{
	TIM5->SR &= ~interrupt;
}

#endif
