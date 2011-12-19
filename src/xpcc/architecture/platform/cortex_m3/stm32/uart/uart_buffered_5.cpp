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

#include <xpcc/architecture/driver/atomic.hpp>
#include <xpcc_config.hpp>

#if defined(STM32F10X_HD) || defined(STM32F10X_XL) || defined(STM32F10X_CL) || \
defined(STM32F2XX) || defined(STM32F4XX)

namespace
{
	static xpcc::atomic::Queue<char, UART5_RX_BUFFER_SIZE> rxBuffer;
	static xpcc::atomic::Queue<char, UART5_TX_BUFFER_SIZE> txBuffer;
}

namespace
{
	GPIO__OUTPUT(TxdC12, C, 12);
	GPIO__INPUT(RxdD2, D, 2);
	
	static const uint32_t apbClk = STM32_APB1_FREQUENCY;	// APB1
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUart5::configurePins(Mapping mapping)
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
xpcc::stm32::BufferedUart5::setBaudrate(uint32_t baudrate)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
	
	UART5->CR1 = 0;
	
	// Enable USART in the interrupt controller and enable receive ready interrupt
	NVIC->ISER[UART5_IRQn / 32] = 1 << (UART5_IRQn & 0x1F);
	UART5->CR1 |= USART_CR1_RXNEIE;
	
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
xpcc::stm32::BufferedUart5::write(const uint8_t *s, uint8_t n)
{
	while (n-- != 0) {
		write(*s++);
	}
}

#endif

// ----------------------------------------------------------------------------
extern "C" void
UART5_IRQHandler(){
	uint32_t state = UART5->SR;
	if (state & USART_SR_RXNE){ // Read Data Register Not Empty 
		// first save the errors TODO
		// error |= USART5_STATUS & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm);
		
		// then read the buffer
		uint8_t data = UART5->DR;
	
		rxBuffer.push(data);
	}
	
	if (state & USART_SR_TXE){ // Transmit Data Register Empty
		if (txBuffer.isEmpty())
		{
			// transmission finished, disable DRE interrupt
			UART5->CR1 &= ~USART_CR1_TXEIE;
		}
		else {
			// get one byte from buffer and write it to the UART buffer
			// which starts the transmission
			UART5->DR = txBuffer.get();
			txBuffer.pop();
		}
	}
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUart5::write(uint8_t c)
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
	UART5->CR1 |= USART_CR1_TXEIE;
}

// ----------------------------------------------------------------------------
bool
xpcc::stm32::BufferedUart5::read(uint8_t& c)
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
xpcc::stm32::BufferedUart5::read(uint8_t *buffer, uint8_t n)
{
	uint8_t i(0);
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
xpcc::stm32::BufferedUart5::flushReceiveBuffer()
{
	uint8_t i(0);
	while(!rxBuffer.isEmpty()) {
		rxBuffer.pop();
		++i;
	}
	unsigned char c;
//	while (USART5_STATUS & USART_RXCIF_bm)
//		c = USART5_DATA;
	
	return i;
}

//uint8_t
//xpcc::stm32::BufferedUart5::flushTransmitBuffer()
//{
//	uint8_t i(0);
//	while(!txBuffer.isEmpty()) {
//		txBuffer.pop();
//		++i;
//	}
//
//	return i;
//}
