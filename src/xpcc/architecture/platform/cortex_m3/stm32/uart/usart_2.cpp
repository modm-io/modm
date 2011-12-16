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

#include "usart_2.hpp"

#include <xpcc_config.hpp>

namespace
{
	GPIO__OUTPUT(TxdA2, A, 2);
	GPIO__INPUT(RxdA3, A, 3);
	
	GPIO__OUTPUT(TxdD5, D, 5);
	GPIO__INPUT(RxdD6, D, 6);
	
	static const uint32_t apbClk = STM32_APB1_FREQUENCY;	// APB1
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Usart2::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F4XX)
	if (mapping == REMAP_PA2_PA3) {
		TxdA2::setAlternateFunction(AF_USART2, xpcc::stm32::PUSH_PULL);
		RxdA3::setAlternateFunction(AF_USART2);
	}
	else {
		TxdD5::setAlternateFunction(AF_USART2, xpcc::stm32::PUSH_PULL);
		RxdD6::setAlternateFunction(AF_USART2);
	}
#else
	AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_USART2_REMAP) | mapping;
	if (mapping == REMAP_PA2_PA3) {
		TxdA2::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdA3::setInput();
	}
	else {
		TxdD5::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdD6::setInput();
	}
#endif
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Usart2::setBaudrate(uint32_t baudrate)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
	// Enable USART in the interrupt controller
	//NVIC->ISER[nvicId / 32] = 1 << (USART2_IRQn & 0x1F);
	
	// set baudrate
	USART2->BRR = calculateBaudrateSettings(apbClk, baudrate);
	
	// Transmitter & Receiver-Enable, 8 Data Bits, 1 Stop Bit
	USART2->CR1 = USART_CR1_TE | USART_CR1_RE;
	USART2->CR2 = 0;
	USART2->CR3 = 0;
	
	USART2->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Usart2::write(char data)
{
	while (!(USART2->SR & USART_SR_TXE)) {
		// wait until the data register becomes empty
	}
	
	USART2->DR = data;
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Usart2::write(const char *s)
{
	char c;
	while ((c = *s++)) {
		write(c);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::stm32::Usart2::read(char& c)
{
	if (USART2->SR & USART_SR_RXNE)
	{
		c = USART2->DR;
		return true;
	}
	
	return false;
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::stm32::Usart2::read(char *buffer, uint8_t n)
{
	for (uint8_t i = 0; i < n; ++i)
	{
		if (read(*buffer++)) {
			return i;
		}
	}
	
	return n;
}


