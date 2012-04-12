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

#include "usart_3.hpp"

#include <xpcc/architecture/driver/atomic.hpp>
#include <xpcc_config.hpp>



namespace
{
	static xpcc::atomic::Queue<char, USART3_RX_BUFFER_SIZE> rxBuffer;
	static xpcc::atomic::Queue<char, USART3_TX_BUFFER_SIZE> txBuffer;
	
	static bool isBlocking = true;
}

namespace
{
	GPIO__OUTPUT(TxdB10, B, 10);
	GPIO__INPUT(RxdB11, B, 11);
	
	GPIO__OUTPUT(TxdD8, D, 8);
	GPIO__INPUT(RxdD9, D, 9);
	
	GPIO__OUTPUT(TxdC10, C, 10);
	GPIO__INPUT(RxdC11, C, 11);
	
	static const uint32_t apbClk = STM32_APB1_FREQUENCY;	// APB1
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart3::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F4XX)
	if (mapping == REMAP_PB10_PB11) {
		TxdB10::setAlternateFunction(AF_USART3, xpcc::stm32::PUSH_PULL);
		RxdB11::setAlternateFunction(AF_USART3);
	}
	else if (mapping == REMAP_PC10_PC11) {
		TxdC10::setAlternateFunction(AF_USART3, xpcc::stm32::PUSH_PULL);
		RxdC11::setAlternateFunction(AF_USART3);
	}
	else {
		TxdD8::setAlternateFunction(AF_USART3, xpcc::stm32::PUSH_PULL);
		RxdD9::setAlternateFunction(AF_USART3);
	}
#else
	AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_USART3_REMAP) | mapping;
	if (mapping == REMAP_PB10_PB11) {
		TxdB10::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdB11::setInput();
	}
	else if (mapping == REMAP_PC10_PC11) {
		TxdC10::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdC11::setInput();
	}
	else {
		TxdD8::setAlternateFunction(xpcc::stm32::PUSH_PULL);
		RxdD9::setInput();
	}
#endif
}

// ----------------------------------------------------------------------------
extern "C" void
USART3_IRQHandler()
{
	uint32_t state = USART3->SR;
	
	// Read Data Register not empty 
	if (state & USART_SR_RXNE)
	{
		// First save the errors TODO
		// error |= USART3_STATUS & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm);
		
		// Then read the buffer (read from DR clears the RXNE flag)
		uint8_t data = USART3->DR;
		
		rxBuffer.push(data);
	}
	
	// Transmit Data Register empty
	if (state & USART_SR_TXE)
	{
		if (txBuffer.isEmpty())
		{
			// transmission finished, disable DRE interrupt
			USART3->CR1 &= ~USART_CR1_TXEIE;
		}
		else {
			// get one byte from buffer and write it to the UART buffer
			// which starts the transmission
			USART3->DR = txBuffer.get();
			txBuffer.pop();
		}
	}
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart3::setBaudrate(uint32_t baudrate,
		uint32_t interruptPriority, bool blocking)
{
	// Enable clock
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	
	isBlocking = blocking;
	
	USART3->CR1 = 0;
	
	// Set vector priority
	NVIC_SetPriority(USART3_IRQn, interruptPriority);
	
	// Enable USART in the interrupt controller and enable receive ready interrupt
	NVIC->ISER[USART3_IRQn / 32] = 1 << (USART3_IRQn & 0x1F);
	
	USART3->CR1 |= USART_CR1_RXNEIE;
	
	// Set baudrate
	USART3->BRR = calculateBaudrateSettings(apbClk, baudrate);
	
	// Transmitter & Receiver-Enable, 8 Data Bits, 1 Stop Bit
	USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;
	USART3->CR2 = 0;
	USART3->CR3 = 0;
	
	USART3->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart3::write(const uint8_t *s, uint8_t n)
{
	while (n-- != 0) {
		write(*s++);
	}
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart3::write(uint8_t c)
{
	while ( !txBuffer.push(c) ) {
		if (!isBlocking) {
			return;
		}
	}
	
	// Disable interrupts while enabling the transmit interrupt
	atomic::Lock lock;
	
	// Transmit Data Register Empty Interrupt Enable
	USART3->CR1 |= USART_CR1_TXEIE;
}

// ----------------------------------------------------------------------------
bool
xpcc::stm32::BufferedUsart3::read(uint8_t& c)
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
xpcc::stm32::BufferedUsart3::read(uint8_t *buffer, uint8_t n)
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
xpcc::stm32::BufferedUsart3::flushReceiveBuffer()
{
	uint_fast8_t i = 0;
	while (!rxBuffer.isEmpty()) {
		rxBuffer.pop();
		++i;
	}
//	unsigned char c;
//	while (USART3_STATUS & USART_RXCIF_bm)
//		c = USART3_DATA;
	
	return i;
}

//uint8_t
//xpcc::stm32::BufferedUsart3::flushTransmitBuffer()
//{
//	uint8_t i(0);
//	while(!txBuffer.isEmpty()) {
//		txBuffer.pop();
//		++i;
//	}
//
//	return i;
//}



