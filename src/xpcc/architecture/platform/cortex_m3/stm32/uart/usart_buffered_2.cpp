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

#include <xpcc/architecture/driver/atomic.hpp>
#include <xpcc_config.hpp>



namespace
{
	static xpcc::atomic::Queue<char, USART2_RX_BUFFER_SIZE> rxBuffer;
	static xpcc::atomic::Queue<char, USART2_TX_BUFFER_SIZE> txBuffer;
}

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
xpcc::stm32::BufferedUsart2::configurePins(Mapping mapping)
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
xpcc::stm32::BufferedUsart2::setBaudrate(uint32_t baudrate)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	
	USART2->CR1 = 0;
	
	// Enable USART in the interrupt controller and enable receive ready interrupt
	NVIC->ISER[USART2_IRQn / 32] = 1 << (USART2_IRQn & 0x1F);
	USART2->CR1 |= USART_CR1_RXNEIE;
	
	// Set baudrate
	USART2->BRR = calculateBaudrateSettings(apbClk, baudrate);
	
	// Transmitter & Receiver-Enable, 8 Data Bits, 1 Stop Bit
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;
	USART2->CR2 = 0;
	USART2->CR3 = 0;
	
	USART2->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart2::write(const uint8_t *s, uint8_t n)
{
	while (n-- != 0) {
		write(*s++);
	}
}

// ----------------------------------------------------------------------------
extern "C" void
USART2_IRQHandler()
{
	uint32_t state = USART2->SR;
	
	// Read Data Register Not Empty 
	if (state & USART_SR_RXNE)
	{
		// first save the errors TODO
		// error |= USART2_STATUS & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm);
		
		// then read the buffer
		uint8_t data = USART2->DR;
	
		rxBuffer.push(data);
	}
	
	// Transmit Data Register Empty
	if (state & USART_SR_TXE)
	{
		if (txBuffer.isEmpty())
		{
			// transmission finished, disable DRE interrupt
			USART2->CR1 &= ~USART_CR1_TXEIE;
		}
		else {
			// get one byte from buffer and write it to the UART buffer
			// which starts the transmission
			USART2->DR = txBuffer.get();
			txBuffer.pop();
		}
	}
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart2::write(uint8_t c)
{
	uint16_t i(0);
	while ( !txBuffer.push(c) && (i < 1) ) {
		++i;
		// wait for a free slot in the buffer
		// but do not wait infinity
	}
	
	// disable interrupts
	atomic::Lock lock;
	
	// Transmit Data Register Empty Interrupt Enable
	USART2->CR1 |= USART_CR1_TXEIE;
}

// ----------------------------------------------------------------------------
bool
xpcc::stm32::BufferedUsart2::read(uint8_t& c)
{
	if (rxBuffer.isEmpty()) {
		return false;
	}
	else {
		c = rxBuffer.get();
		rxBuffer.pop();
		
		return true;
	}
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::stm32::BufferedUsart2::read(uint8_t *buffer, uint8_t n)
{
	uint_fast8_t i = 0;
	for (; i < n; ++i)
	{
		if (rxBuffer.isEmpty()) {
			return i;
		}
		else {
			*buffer++ = rxBuffer.get();
			rxBuffer.pop();
		}
	}
	
	return i;
}

uint8_t
xpcc::stm32::BufferedUsart2::flushReceiveBuffer()
{
	uint_fast8_t i = 0;
	while (!rxBuffer.isEmpty()) {
		rxBuffer.pop();
		++i;
	}
//	unsigned char c;
//	while (USART2_STATUS & USART_RXCIF_bm)
//		c = USART2_DATA;
	
	return i;
}

//uint8_t
//xpcc::stm32::BufferedUsart2::flushTransmitBuffer()
//{
//	uint8_t i(0);
//	while(!txBuffer.isEmpty()) {
//		txBuffer.pop();
//		++i;
//	}
//
//	return i;
//}



