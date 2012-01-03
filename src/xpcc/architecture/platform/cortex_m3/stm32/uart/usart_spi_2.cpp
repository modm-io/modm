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

#if defined(STM32F2XX) || defined(STM32F4XX)

// ----------------------------------------------------------------------------
xpcc::stm32::UsartSpi2::UsartSpi2(
		uint32_t bitrate, Mode mode)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
	USART2->CR1 = 0;
	
	// Set baudrate
	USART2->BRR = calculateBaudrateSettings(STM32_APB1_FREQUENCY, bitrate);
	
	USART2->CR2 = USART_CR2_CLKEN | USART_CR2_LBCL | mode;
	USART2->CR3 = 0;
	
	// Transmitter & Receiver-Enable, 8 Data Bits, 1 Stop Bit
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
	USART2->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::stm32::UsartSpi2::write(uint8_t data)
{
	USART2->DR = data;
	while (!(USART2->SR & USART_SR_RXNE)) {
		// wait until the transmission is finished
	}
	return USART2->DR;
}

#endif
