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
	GPIO__OUTPUT(SckA5, A, 5);
	GPIO__INPUT(MisoA6, A, 6);
	GPIO__OUTPUT(MosiA7, A, 7);
	
	GPIO__OUTPUT(SckB3, B, 3);
	GPIO__INPUT(MisoB4, B, 4);
	GPIO__OUTPUT(MosiB5, B, 5);
	
	static uint8_t* transmitBuffer(0);
	static uint8_t* receiveBuffer(0);
	static uint16_t bufferLength(0);
	enum
	{
		BUFFER_TRANSMIT_INCR_bm = 0x01,
		BUFFER_RECEIVE_INCR_bm = 0x02,
		BUFFER_TRANSMIT_IS_NOT_ZERO_bm = 0x04,
		BUFFER_RECEIVE_IS_NOT_ZERO_bm = 0x08,
		BUFFER_IS_DUMMY_bm = 0x10,
		BUFFER_IS_BUSY_SYNC_bm = 0x20
	};
	static uint8_t status(0);
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::SpiMaster1::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F4XX)
	if (mapping == REMAP_PA5_PA6_PA7) {
		SckA5::setAlternateFunction(AF_SPI1, xpcc::stm32::PUSH_PULL);
		MisoA6::setAlternateFunction(AF_SPI1);
		MosiA7::setAlternateFunction(AF_SPI1, xpcc::stm32::PUSH_PULL);
	}
	else {
		SckB3::setAlternateFunction(AF_SPI1, xpcc::stm32::PUSH_PULL);
		MisoB4::setAlternateFunction(AF_SPI1);
		MosiB5::setAlternateFunction(AF_SPI1, xpcc::stm32::PUSH_PULL);
	}
#else
	AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_SPI1_REMAP) | mapping;
	if (mapping == REMAP_PA5_PA6_PA7) {
		SckA5::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		MisoA6::setInput(xpcc::stm32::FLOATING);
		MosiA7::setAlternateFunction(xpcc::stm32::PUSH_PULL);
	}
	else {
		SckB3::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		MisoB4::setInput(xpcc::stm32::FLOATING);
		MosiB5::setAlternateFunction(xpcc::stm32::PUSH_PULL);
	}
#endif
}


// ----------------------------------------------------------------------------
void
xpcc::stm32::SpiMaster1::initialize(Mode mode, Prescaler prescaler)
{
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	RCC->APB2RSTR |=  RCC_APB2RSTR_SPI1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
	
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
bool
xpcc::stm32::SpiMaster1::setBuffer(uint16_t length, uint8_t* transmit, uint8_t* receive, bool transmitIncr, bool receiveIncr)
{
	if (!isFinished()) {
		return false;
	}
	
	transmitBuffer = transmit;
	receiveBuffer = receive ? receive : transmit;
	bufferLength = length;
	
	status &= ~(BUFFER_TRANSMIT_INCR_bm | BUFFER_RECEIVE_INCR_bm);
	status |= (transmitIncr ? BUFFER_TRANSMIT_INCR_bm : 0) | (receiveIncr ? BUFFER_RECEIVE_INCR_bm : 0);
	
	return true;
}

bool
xpcc::stm32::SpiMaster1::transfer(bool transmit, bool receive, bool /*wait*/)
{
	if (status & BUFFER_IS_BUSY_SYNC_bm) {
		return false;
	}
	
	uint8_t rx = 0;
	uint8_t tx = 0xff;
	
	// send the buffer out, blocking
	status |= BUFFER_IS_BUSY_SYNC_bm;
	
	// check if we have to use a dummy buffer
	transmit &= static_cast<bool>(transmitBuffer);
	receive &= static_cast<bool>(receiveBuffer);
	
	if (status & BUFFER_TRANSMIT_INCR_bm) {
		for (uint_fast16_t i = 0; i < bufferLength; ++i) {
			if (transmit) {
				tx = transmitBuffer[i];
			}
			rx = write(tx);
			if (receive) receiveBuffer[i] = rx;
		}
	}
	else {
		for (uint_fast16_t i = bufferLength; i > 0; --i) {
			if (transmit) {
				tx = transmitBuffer[i-1];
			}
			rx = write(tx);
			if (receive) receiveBuffer[i-1] = rx;
		}
	}
	status &= ~BUFFER_IS_BUSY_SYNC_bm;
	
	return true;
}

bool
xpcc::stm32::SpiMaster1::isFinished()
{
	return !(status & BUFFER_IS_BUSY_SYNC_bm);
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::stm32::SpiMaster1::write(uint8_t data)
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

