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

#if defined(STM32F10X_HD) || defined(STM32F10X_XL) || defined(STM32F10X_CL) || \
	defined(STM32F2XX) || defined(STM32F3XXV) || defined(STM32F4XX)
// only devices with 100 pins have Port D and therefore UART5


#include "uart_hal_5.hpp"

#include <xpcc_config.hpp>


namespace
{
	GPIO__OUTPUT(TxdC12, C, 12);
	GPIO__INPUT(RxdD2, D, 2);
}
namespace
{
	static const uint32_t apbClk = STM32_APB1_FREQUENCY;	// APB1
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UartHal5::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F3XX) || defined(STM32F4XX)
	(void) mapping;		// avoid compiler warning
	
	TxdC12::setAlternateFunction(AF_UART5, xpcc::stm32::PUSH_PULL);
	RxdD2::setAlternateFunction(AF_UART5);
#else
	(void) mapping;		// avoid compiler warning
	
	TxdC12::setAlternateFunction(xpcc::stm32::PUSH_PULL);
	RxdD2::setInput();
#endif
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UartHal5::setBaudrate(uint32_t baudrate)
{
	UART5->BRR =	calculateBaudrateSettings(apbClk, baudrate);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UartHal5::enable()
{
// FIXME: there seems to be a bug in the stm32f3xxlib which does not provide
	//        the necessary RCC_APB1ENR_UART5EN define and probably defines
	//        RCC_APB1ENR_UART4EN incorrectly (0x00100000 instead of 0x00080000)
	// enable clock
	RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
	// reset uart
	RCC->APB1RSTR |=  RCC_APB1RSTR_UART5RST;
	RCC->APB1RSTR &= ~RCC_APB1RSTR_UART5RST;

	UART5->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UartHal5::disable()
{
	UART5->CR1 = 0;		// TX, RX, Uart, etc. Disable

// FIXME: there seems to be a bug in the stm32f3xxlib which does not provide
	//        the necessary RCC_APB1ENR_UART5EN define and probably defines
	//        RCC_APB1ENR_UART4EN incorrectly (0x00100000 instead of 0x00080000)
	// disable clock
	RCC->APB1ENR &= ~RCC_APB1ENR_UART5EN;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::UartHal5::enableInterruptVector(bool enable,
uint32_t priority)
{
	if (enable) {
		// Set priority for the interrupt vector
		NVIC_SetPriority(UART5_IRQn, priority);
		
		// register IRQ at the NVIC
		NVIC_EnableIRQ(UART5_IRQn);
	}
	else {
		NVIC_DisableIRQ(UART5_IRQn);
	}
}

#endif
