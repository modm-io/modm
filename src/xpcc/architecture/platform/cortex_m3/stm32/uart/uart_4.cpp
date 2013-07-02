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

#include "uart_4.hpp"

#include <xpcc_config.hpp>

#if defined(STM32F10X_HD) || defined(STM32F10X_XL) || defined(STM32F10X_CL) || \
	defined(STM32F2XX) || defined(STM32F3XX) || defined(STM32F4XX)

namespace
{
	#if defined(STM32F2XX) || defined(STM32F4XX)
	GPIO__OUTPUT(TxdA0, A, 0);
	GPIO__INPUT(RxdA1, A, 1);
#endif
	
	GPIO__OUTPUT(TxdC10, C, 10);
	GPIO__INPUT(RxdC11, C, 11);
}
namespace
{
	static const uint32_t apbClk = STM32_APB1_FREQUENCY;	// APB1
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Uart4::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F3XX) || defined(STM32F4XX)
	#if defined(STM32F3XX)
	(void) mapping;		// avoid compiler warning

	TxdC10::setAlternateFunction(AF_UART4, xpcc::stm32::PUSH_PULL);
	RxdC11::setAlternateFunction(AF_UART4);
#else
	if (mapping == REMAP_PA0_PA1) {
		TxdA0::setAlternateFunction(AF_UART4, xpcc::stm32::PUSH_PULL);
		RxdA1::setAlternateFunction(AF_UART4);
	}
	else {
		TxdC10::setAlternateFunction(AF_UART4, xpcc::stm32::PUSH_PULL);
		RxdC11::setAlternateFunction(AF_UART4);
	}
#endif
#else
	(void) mapping;		// avoid compiler warning
	
	TxdC10::setAlternateFunction(xpcc::stm32::PUSH_PULL);
	RxdC11::setInput();
#endif
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Uart4::setBaudrate(uint32_t baudrate)
{
	// Enable clock
	// FIXME: there seems to be a bug in the stm32f3xxlib which does not provide
	//        the necessary RCC_APB1ENR_UART5EN define and probably defines
	//        RCC_APB1ENR_UART4EN incorrectly (0x00100000 instead of 0x00080000)
	RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
	
	UART4->CR1 = 0;
	
	// Set baudrate
	UART4->BRR = calculateBaudrateSettings(apbClk, baudrate);
	
	// Transmitter & Receiver-Enable, 8 Data Bits, 1 Stop Bit
	UART4->CR1 |= USART_CR1_TE | USART_CR1_RE;
	UART4->CR2 = 0;
	UART4->CR3 = 0;
	
	UART4->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Uart4::write(uint8_t data)
{
#if defined(STM32F3XX)
	while (!(UART4->ISR & USART_ISR_TXE)) {
		// wait until the data register becomes empty
	}

	UART4->TDR = data;
#else
	while (!(UART4->SR & USART_SR_TXE)) {
		// wait until the data register becomes empty
	}
	
	UART4->DR = data;
#endif
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Uart4::write(const uint8_t *s, uint8_t n)
{
	while (n-- != 0) {
		write(*s++);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::stm32::Uart4::read(uint8_t& c)
{
#if defined(STM32F3XX)
	if (UART4->ISR & USART_ISR_RXNE)
	{
		c = UART4->RDR;
		return true;
	}
#else
	if (UART4->SR & USART_SR_RXNE)
	{
		c = UART4->DR;
		return true;
	}
#endif
	return false;
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::stm32::Uart4::read(uint8_t *buffer, uint8_t n)
{
	for (uint8_t i = 0; i < n; ++i)
	{
		if (read(*buffer++)) {
			return i;
		}
	}
	
	return n;
}

#endif
