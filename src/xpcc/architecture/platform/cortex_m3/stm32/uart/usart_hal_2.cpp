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



#include "usart_hal_2.hpp"

#include <xpcc_config.hpp>


namespace
{
	GPIO__OUTPUT(TxdA2, A, 2);
	GPIO__INPUT(RxdA3, A, 3);
	
	GPIO__OUTPUT(TxdD5, D, 5);
	GPIO__INPUT(RxdD6, D, 6);

#if defined(STM32F3XX)
	GPIO__OUTPUT(TxdA14, A, 14);
	GPIO__INPUT(RxdA15, A, 15);

	GPIO__OUTPUT(TxdB3, B, 3);
	GPIO__INPUT(RxdB4, B, 4);
#endif
}
namespace
{
	static const uint32_t apbClk = STM32_APB1_FREQUENCY;	// APB1
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal2::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F3XX) || defined(STM32F4XX)
	if (mapping == REMAP_PA2_PA3) {
		TxdA2::setAlternateFunction(AF_USART2, xpcc::stm32::PUSH_PULL);
		RxdA3::setAlternateFunction(AF_USART2);
	}
#if defined(STM32F3XX)
	else if(mapping == REMAP_PA14_PA15) {
		TxdA14::setAlternateFunction(AF_USART2, xpcc::stm32::PUSH_PULL);
		RxdA15::setAlternateFunction(AF_USART2);
	}
	else if(mapping == REMAP_PB3_PB4) {
		TxdB3::setAlternateFunction(AF_USART2, xpcc::stm32::PUSH_PULL);
		RxdB4::setAlternateFunction(AF_USART2);
	}
#endif
	else {
		TxdD5::setAlternateFunction(AF_USART2, xpcc::stm32::PUSH_PULL);
		RxdD6::setAlternateFunction(AF_USART2);
	}
#elif defined(STM32F10X) || defined(STM32F10X_HD) || defined(STM32F10X_XL) \
	|| defined(STM32F10X_CL)
	AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_USART2_REMAP) | mapping;
	if (mapping == REMAP_PA2_PA3) {
		TxdA2::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdA3::setInput();
	}
	else {
		TxdD5::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdD6::setInput();
	}
#else
#  error "Unknown CPU Type. Please define STM32F10X_.., STM32F2XX, STM32F3XX or STM32F4XX"
#endif
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal2::setBaudrate(uint32_t baudrate)
{
	USART2->BRR =	calculateBaudrateSettings(apbClk, baudrate);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal2::enable()
{
// enable clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	// reset timer
	RCC->APB1RSTR |=  RCC_APB1RSTR_USART2RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_USART2RST;

	USART2->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal2::disable()
{
	USART2->CR1 = 0;		// TX, RX, Uart, etc. Disable

// disable clock
	RCC->APB1ENR &= ~RCC_APB1ENR_USART2EN;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartHal2::enableInterruptVector(bool enable,
uint32_t priority)
{
	if (enable) {
		// Set priority for the interrupt vector
		NVIC_SetPriority(USART2_IRQn, priority);
		
		// register IRQ at the NVIC
		NVIC_EnableIRQ(USART2_IRQn);
	}
	else {
		NVIC_DisableIRQ(USART2_IRQn);
	}
}


