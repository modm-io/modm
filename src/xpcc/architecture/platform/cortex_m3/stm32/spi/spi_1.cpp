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
#include "spi_1.hpp"

namespace
{
	GPIO__OUTPUT(Cs, A, 4);		// Remap A15
	GPIO__OUTPUT(Sck, A, 5);	// Remap B3
	GPIO__INPUT(Miso, A, 6);	// Remap B4
	GPIO__OUTPUT(Mosi, A, 7);	// Remap B5
	
	static const uint32_t apbId = 12;	// APB2
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Spi1::initialize(Mode mode, Prescaler prescaler)
{
#if defined(STM32F2XX) || defined(STM32F4XX)

#else
	Cs::setAlternateFunction(xpcc::stm32::PUSH_PULL);
	Cs::set();
	Sck::setAlternateFunction(xpcc::stm32::PUSH_PULL);
	Miso::setInput(xpcc::stm32::FLOATING);
	Mosi::setAlternateFunction(xpcc::stm32::PUSH_PULL);
#endif
	
	RCC->APB2ENR |= (1 << apbId);
	RCC->APB2RSTR |= (1 << apbId);
	RCC->APB2RSTR &= ~(1 << apbId);
	
	// disable all interrupts
	SPI1->CR2 &= ~(SPI_CR2_TXEIE  | SPI_CR2_RXNEIE  | SPI_CR2_ERRIE);
	
	// disable peripheral
	SPI1->CR1 &= ~SPI_CR1_SPE;
	
	// set new mode
	SPI1->CR1 = prescaler | mode | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
	    
	// reenable peripheral
	SPI1->CR1 |= SPI_CR1_SPE;
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::stm32::Spi1::write(uint8_t data)
{
	while (!(SPI1->SR & SPI_SR_TXE)) {
		// wait until the previous transmission is finished
	}
	
	SPI1->DR = data;
	
	while (!(SPI1->SR & SPI_SR_RXNE)) {
		// wait until the data is received
	}
	
	return SPI1->DR;
}

