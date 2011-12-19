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

#include "uart_5.hpp"

#include <xpcc_config.hpp>

#if defined(STM32F10X_HD) || defined(STM32F10X_XL) || defined(STM32F10X_CL) || \
defined(STM32F2XX) || defined(STM32F4XX)

namespace
{
	GPIO__OUTPUT(TxdC12, C, 12);
	GPIO__INPUT(RxdD2, D, 2);
	
	static const uint32_t apbClk = STM32_APB1_FREQUENCY;	// APB1
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Uart5::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F4XX)
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
xpcc::stm32::Uart5::setBaudrate(uint32_t baudrate)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
	
	UART5->CR1 = 0;
	
	// Set baudrate
	UART5->BRR = calculateBaudrateSettings(apbClk, baudrate);
	
	// Transmitter & Receiver-Enable, 8 Data Bits, 1 Stop Bit
	UART5->CR1 |= USART_CR1_TE | USART_CR1_RE;
	UART5->CR2 = 0;
	UART5->CR3 = 0;
	
	UART5->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Uart5::write(uint8_t data)
{
	while (!(UART5->SR & USART_SR_TXE)) {
		// wait until the data register becomes empty
	}
	
	UART5->DR = data;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Uart5::write(const uint8_t *s, uint8_t n)
{
	while (n-- != 0) {
		write(*s++);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::stm32::Uart5::read(uint8_t& c)
{
	if (UART5->SR & USART_SR_RXNE)
	{
		c = UART5->DR;
		return true;
	}
	
	return false;
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::stm32::Uart5::read(uint8_t *buffer, uint8_t n)
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
