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

#include "usart_1.hpp"

#include <xpcc_config.hpp>

namespace
{
	static const uint32_t apbClk = STM32_APB2_FREQUENCY;	// APB2
}

#if defined(STM32F2XX) || defined(STM32F4XX)

// ----------------------------------------------------------------------------
void
xpcc::stm32::UsartSpi1::initialize(
		uint32_t bitrate, Mode mode, bool over8)
{
	// Enable clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	USART1->CR1 = 0;
	
	// Set baudrate
	if (over8) {
		const uint16_t brr = calculateBaudrateSettings(apbClk, bitrate/2);
		USART1->CR1 |= USART_CR1_OVER8;
		USART1->BRR = (USART_BRR_DIV_Mantissa & brr) | ((USART_BRR_DIV_Fraction & brr)>>1);
	}
	else {
		USART1->BRR = calculateBaudrateSettings(apbClk, bitrate);
	}
	
	USART1->CR2 = USART_CR2_CLKEN | USART_CR2_LBCL | mode;
	USART1->CR3 = 0;
	
	// Transmitter & Receiver-Enable, 8 Data Bits, 1 Stop Bit
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
	USART1->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::stm32::UsartSpi1::write(uint8_t data)
{
	USART1->DR = __REV(__RBIT(data));
	while (!(USART1->SR & USART_SR_RXNE)) {
		// wait until the transmission is finished
	}
	return __REV(__RBIT(USART1->DR));
}

#endif
