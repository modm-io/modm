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

#include "usart_2.hpp"

namespace
{
	GPIO__OUTPUT(Txd, A, 2);		// Remap D5
	GPIO__INPUT(Rxd, A, 3);			// Remap D6
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Usart2::setBaudrate(uint32_t baudrate)
{
	Txd::setOutput(xpcc::stm32::ALTERNATE, xpcc::stm32::PUSH_PULL);
	Rxd::setInput(xpcc::stm32::INPUT, xpcc::stm32::FLOATING);
	
	//rcc_clk_enable(RCC_USART2);
	//nvic_irq_enable(NVIC_USART2);
	
	// set baudrate
	USART2->BRR = calculateBaudrateSettings(36e6, baudrate);
	
	// Transmitter & Receiver-Enable, 8 Data Bits, 1 Stop Bit
	USART2->CR1  = USART_CR1_TE | USART_CR1_RE;
	USART2->CR2  = 0;
	USART2->CR3  = 0;
	
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

