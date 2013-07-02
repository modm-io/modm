// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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



#include "usart_hal_1.hpp"

#include <xpcc_config.hpp>


namespace
{
	GPIO__OUTPUT(TxdA9, A, 9);
	GPIO__INPUT(RxdA10, A, 10);
	
	GPIO__OUTPUT(TxdB6, B, 6);
	GPIO__INPUT(RxdB7, B, 7);
}
namespace
{
	static const uint32_t apbClk = STM32_APB2_FREQUENCY;	// APB2
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal1::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F3XX) || defined(STM32F4XX)
	if (mapping == REMAP_PA9_PA10) {
		TxdA9::setAlternateFunction(AF_USART1, xpcc::stm32::PUSH_PULL);
		RxdA10::setAlternateFunction(AF_USART1);
	}
	else {
		TxdB6::setAlternateFunction(AF_USART1, xpcc::stm32::PUSH_PULL);
		RxdB7::setAlternateFunction(AF_USART1);
	}
#elif defined(STM32F10X) || defined(STM32F10X_HD) || defined(STM32F10X_XL) \
	|| defined(STM32F10X_CL)
	AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_USART1_REMAP) | mapping;
	if (mapping == REMAP_PA9_PA10) {
		TxdA9::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdA10::setInput();
	}
	else {
		TxdB6::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdB7::setInput();
	}
#else
#  error "Unknown CPU Type. Please define STM32F10X_.., STM32F2XX, STM32F3XX or STM32F4XX"
#endif
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal1::setBaudrate(uint32_t baudrate)
{
	USART1->BRR =	calculateBaudrateSettings(apbClk, baudrate);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal1::enable()
{
// enable clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	// reset timer
	RCC->APB2RSTR |=  RCC_APB2RSTR_USART1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_USART1RST;

	USART1->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal1::disable()
{
	USART1->CR1 = 0;		// TX, RX, Uart, etc. Disable

// disable clock
	RCC->APB2ENR &= ~RCC_APB2ENR_USART1EN;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal1::enableInterruptVector(bool enable,
uint32_t priority)
{
	if (enable) {
		// Set priority for the interrupt vector
		NVIC_SetPriority(USART1_IRQn, priority);
		
		// register IRQ at the NVIC
		NVIC_EnableIRQ(USART1_IRQn);
	}
	else {
		NVIC_DisableIRQ(USART1_IRQn);
	}
}


