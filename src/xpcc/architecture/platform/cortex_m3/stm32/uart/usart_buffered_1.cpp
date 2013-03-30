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

#include <xpcc/architecture/driver/atomic.hpp>
#include <xpcc_config.hpp>



namespace
{
	static xpcc::atomic::Queue<char, USART1_RX_BUFFER_SIZE> rxBuffer;
	static xpcc::atomic::Queue<char, USART1_TX_BUFFER_SIZE> txBuffer;
	
	static bool isBlocking = true;
}

namespace
{
	GPIO__OUTPUT(TxdA9, A, 9);
	GPIO__INPUT(RxdA10, A, 10);
	
	GPIO__OUTPUT(TxdB6, B, 6);
	GPIO__INPUT(RxdB7, B, 7);
}
namespace
{
	static const uint32_t apbClk = STM32_APB2_FREQUENCY;	// APB2
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart1::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F4XX)
	if (mapping == REMAP_PA9_PA10) {
		TxdA9::setAlternateFunction(AF_USART1, xpcc::stm32::PUSH_PULL);
		RxdA10::setAlternateFunction(AF_USART1);
	}
	else {
		TxdB6::setAlternateFunction(AF_USART1, xpcc::stm32::PUSH_PULL);
		RxdB7::setAlternateFunction(AF_USART1);
	}
#else
	AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_USART1_REMAP) | mapping;
	if (mapping == REMAP_PA9_PA10) {
		TxdA9::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdA10::setInput();
	}
	else {
		TxdB6::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdB7::setInput();
	}
#endif
}

// ----------------------------------------------------------------------------
extern "C" void
USART1_IRQHandler()
{
	uint32_t state = USART1->SR;
	
	// Read Data Register not empty 
	if (state & USART_SR_RXNE)
	{
		// First save the errors TODO
		// error |= USART1_STATUS & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm);
		
		// Then read the buffer (read from DR clears the RXNE flag)
		uint8_t data = USART1->DR;
		
		rxBuffer.push(data);
	}
	
	// Transmit Data Register empty
	if (state & USART_SR_TXE)
	{
		if (txBuffer.isEmpty())
		{
			// transmission finished, disable DRE interrupt
			USART1->CR1 &= ~USART_CR1_TXEIE;
		}
		else {
			// get one byte from buffer and write it to the UART buffer
			// which starts the transmission
			USART1->DR = txBuffer.get();
			txBuffer.pop();
		}
	}
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart1::setBaudrate(uint32_t baudrate,
		uint32_t interruptPriority, bool blocking)
{
	// Enable clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	isBlocking = blocking;
	
	USART1->CR1 = 0;
	
	// Set vector priority
	NVIC_SetPriority(USART1_IRQn, interruptPriority);
	
	// Enable USART in the interrupt controller and enable receive ready interrupt
	NVIC->ISER[USART1_IRQn / 32] = 1 << (USART1_IRQn & 0x1F);
	
	USART1->CR1 |= USART_CR1_RXNEIE;
	
	// Set baudrate
	USART1->BRR = calculateBaudrateSettings(apbClk, baudrate);
	
	// Transmitter & Receiver-Enable, 8 Data Bits, 1 Stop Bit
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
	USART1->CR2 = 0;
	USART1->CR3 = 0;
	
	USART1->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart1::write(const uint8_t *s, uint8_t n)
{
	while (n-- != 0) {
		write(*s++);
	}
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart1::write(uint8_t c)
{
	while ( !txBuffer.push(c) ) {
		if (!isBlocking) {
			return;
		}
	}
	
	// Disable interrupts while enabling the transmit interrupt
	atomic::Lock lock;
	
	// Transmit Data Register Empty Interrupt Enable
	USART1->CR1 |= USART_CR1_TXEIE;
}

// ----------------------------------------------------------------------------
bool
xpcc::stm32::BufferedUsart1::read(uint8_t& c)
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
xpcc::stm32::BufferedUsart1::read(uint8_t *buffer, uint8_t n)
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
xpcc::stm32::BufferedUsart1::flushReceiveBuffer()
{
	uint_fast8_t i = 0;
	while (!rxBuffer.isEmpty()) {
		rxBuffer.pop();
		++i;
	}
//	unsigned char c;
//	while (USART1_STATUS & USART_RXCIF_bm)
//		c = USART1_DATA;
	
	return i;
}

//uint8_t
//xpcc::stm32::BufferedUsart1::flushTransmitBuffer()
//{
//	uint8_t i(0);
//	while(!txBuffer.isEmpty()) {
//		txBuffer.pop();
//		++i;
//	}
//
//	return i;
//}



