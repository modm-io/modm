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
#include "spi_3.hpp"

#ifndef STM32F10X_LD

namespace
{
	GPIO__OUTPUT(Cs, A, 15);
	GPIO__OUTPUT(Sck, B, 3);
	GPIO__INPUT(Miso, B, 4);
	GPIO__OUTPUT(Mosi, B, 5);
	
	static const uint32_t apbId = 15;	// APB1
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::Spi3::initialize(Mode mode, Prescaler prescaler)
{
#if defined(STM32F2XX) || defined(STM32F4XX)

#else
	Cs::setAlternateFunction(xpcc::stm32::PUSH_PULL);
	Cs::set();
	Sck::setAlternateFunction(xpcc::stm32::PUSH_PULL);
	Miso::setInput(xpcc::stm32::FLOATING);
	Mosi::setAlternateFunction(xpcc::stm32::PUSH_PULL);
#endif
	
	RCC->APB1ENR |= (1 << apbId);
	RCC->APB1RSTR |= (1 << apbId);
	RCC->APB1RSTR &= ~(1 << apbId);
	
	// disable all interrupts
	SPI3->CR2 &= ~(SPI_CR2_TXEIE  | SPI_CR2_RXNEIE  | SPI_CR2_ERRIE);
	
	// disable peripheral
	SPI3->CR1 &= ~SPI_CR1_SPE;
	
	// set new mode
	SPI3->CR1 = prescaler | mode | SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;
	    
	// reenable peripheral
	SPI3->CR1 |= SPI_CR1_SPE;
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::stm32::Spi3::write(uint8_t data)
{
	while (!(SPI3->SR & SPI_SR_TXE)) {
		// wait until the previous transmission is finished
	}
	
	SPI3->DR = data;
	
	while (!(SPI3->SR & SPI_SR_RXNE)) {
		// wait until the data is received
	}
	
	return SPI3->DR;
}

#endif