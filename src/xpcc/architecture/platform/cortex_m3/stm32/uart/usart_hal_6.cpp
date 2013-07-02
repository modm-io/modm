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


#if defined(STM32F2XX) || defined(STM32F4XX)


#include "usart_hal_6.hpp"

#include <xpcc_config.hpp>


namespace
{
	GPIO__OUTPUT(TxdC6, C, 6);
	GPIO__INPUT(RxdC7, C, 7);
	
	GPIO__OUTPUT(TxdG14, G, 14);
	GPIO__INPUT(RxdG9, G, 9);
}
namespace
{
	static const uint32_t apbClk = STM32_APB2_FREQUENCY;	// APB2
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal6::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F3XX) || defined(STM32F4XX)
	if (mapping == REMAP_PC6_PC7) {
		TxdC6::setAlternateFunction(AF_USART6, xpcc::stm32::PUSH_PULL);
		RxdC7::setAlternateFunction(AF_USART6);
	}
	else {
		TxdG14::setAlternateFunction(AF_USART6, xpcc::stm32::PUSH_PULL);
		RxdG9::setAlternateFunction(AF_USART6);
	}
#elif defined(STM32F10X) || defined(STM32F10X_HD) || defined(STM32F10X_XL) \
	|| defined(STM32F10X_CL)
	AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_USART6_REMAP) | mapping;
	
#else
#  error "Unknown CPU Type. Please define STM32F10X_.., STM32F2XX, STM32F3XX or STM32F4XX"
#endif
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal6::setBaudrate(uint32_t baudrate)
{
	USART6->BRR =	calculateBaudrateSettings(apbClk, baudrate);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal6::enable()
{
// enable clock
	RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	// reset timer
	RCC->APB2RSTR |=  RCC_APB2RSTR_USART6RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_USART6RST;

	USART6->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal6::disable()
{
	USART6->CR1 = 0;		// TX, RX, Uart, etc. Disable

// disable clock
	RCC->APB2ENR &= ~RCC_APB2ENR_USART6EN;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal6::enableInterruptVector(bool enable,
uint32_t priority)
{
	if (enable) {
		// Set priority for the interrupt vector
		NVIC_SetPriority(USART6_IRQn, priority);
		
		// register IRQ at the NVIC
		NVIC_EnableIRQ(USART6_IRQn);
	}
	else {
		NVIC_DisableIRQ(USART6_IRQn);
	}
}

#endif
