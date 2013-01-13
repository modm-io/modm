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

#include "usart_6.hpp"

#include <xpcc/architecture/driver/atomic.hpp>
#include <xpcc_config.hpp>


#if defined(STM32F2XX) || defined(STM32F4XX)


namespace
{
	static xpcc::atomic::Queue<char, USART6_RX_BUFFER_SIZE> rxBuffer;
	static xpcc::atomic::Queue<char, USART6_TX_BUFFER_SIZE> txBuffer;
	
	static bool isBlocking = true;
}

namespace
{
	GPIO__OUTPUT(TxdC6, C, 6);
	GPIO__INPUT(RxdC7, C, 7);
	
	GPIO__OUTPUT(TxdG14, G, 14);
	GPIO__INPUT(RxdG9, G, 9);
	
	static const uint32_t apbClk = STM32_APB2_FREQUENCY;	// APB2
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart6::configurePins(Mapping mapping)
{
	// Enable clock
	RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	
	// Initialize IO pins
#if defined(STM32F2XX) || defined(STM32F3XX) || defined(STM32F4XX)
	if (mapping == REMAP_PC6_PC7) {
		TxdC6::setAlternateFunction(AF_USART6, xpcc::stm32::PUSH_PULL);
		RxdC7::setAlternateFunction(AF_USART6);
	}
	else {
		TxdG14::setAlternateFunction(AF_USART6, xpcc::stm32::PUSH_PULL);
		RxdG9::setAlternateFunction(AF_USART6);
	}
#elif defined(STM32F10X) || defined(STM32F10X_HD) || defined(STM32F10X_XL) \
	|| defined(STM32F10X_CL)
	AFIO->MAPR = (AFIO->MAPR & ~AFIO_MAPR_USART6_REMAP) | mapping;
	
#else
#  error "Unknown CPU Type. Please define STM32F10X_.., STM32F2XX, STM32F3XX or STM32F4XX"
#endif
}

// ----------------------------------------------------------------------------
#if defined(STM32F3XX)
	#define USART_SR_RXNE USART_ISR_RXNE
	#define USART_SR_TXE  USART_ISR_TXE

	// new register names for STM32F3 series
	#define USART6_SR \
		USART6->ISR
	#define USART6_RDR \
		USART6->RDR
	#define USART6_TDR \
		USART6->TDR

#else
	#define USART6_SR \
		USART6->SR
	#define USART6_RDR \
		USART6->DR
	#define USART6_TDR \
		USART6->DR
#endif

extern "C" void
USART6_IRQHandler()
{

	uint32_t state = USART6_SR;
	
	// Read Data Register not empty 
	if (state & USART_SR_RXNE)
	{
		// First save the errors TODO
		// error |= USART6_STATUS & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm);
		
		// Then read the buffer (read from DR clears the RXNE flag)
		uint8_t data = USART6_RDR;
		
		rxBuffer.push(data);
	}
	
	// Transmit Data Register empty
	if (state & USART_SR_TXE)
	{
		if (txBuffer.isEmpty())
		{
			// transmission finished, disable DRE interrupt
			USART6->CR1 &= ~USART_CR1_TXEIE;
		}
		else {
			// get one byte from buffer and write it to the UART buffer
			// which starts the transmission
			USART6_TDR = txBuffer.get();
			txBuffer.pop();
		}
	}
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart6::setBaudrate(uint32_t baudrate,
		uint32_t interruptPriority, bool blocking)
{
	// Enable clock
	RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
	
	isBlocking = blocking;
	
	USART6->CR1 = 0;
	
	// Set vector priority
	NVIC_SetPriority(USART6_IRQn, interruptPriority);
	
	// Enable USART in the interrupt controller and enable receive ready interrupt
	NVIC->ISER[USART6_IRQn / 32] = 1 << (USART6_IRQn & 0x1F);
	
	USART6->CR1 |= USART_CR1_RXNEIE;
	
	// Set baudrate
	USART6->BRR = calculateBaudrateSettings(apbClk, baudrate);
	
	// Transmitter & Receiver-Enable, 8 Data Bits, 1 Stop Bit
	USART6->CR1 |= USART_CR1_TE | USART_CR1_RE;
	USART6->CR2 = 0;
	USART6->CR3 = 0;
	
	USART6->CR1 |= USART_CR1_UE;		// Uart Enable
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart6::write(const uint8_t *s, uint8_t n)
{
	while (n-- != 0) {
		write(*s++);
	}
}

// ----------------------------------------------------------------------------
void
xpcc::stm32::BufferedUsart6::write(uint8_t c)
{
	while ( !txBuffer.push(c) ) {
		if (!isBlocking) {
			return;
		}
	}
	
	// Disable interrupts while enabling the transmit interrupt
	atomic::Lock lock;
	
	// Transmit Data Register Empty Interrupt Enable
	USART6->CR1 |= USART_CR1_TXEIE;
}

// ----------------------------------------------------------------------------
bool
xpcc::stm32::BufferedUsart6::read(uint8_t& c)
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
xpcc::stm32::BufferedUsart6::read(uint8_t *buffer, uint8_t n)
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
xpcc::stm32::BufferedUsart6::flushReceiveBuffer()
{
	uint_fast8_t i = 0;
	while (!rxBuffer.isEmpty()) {
		rxBuffer.pop();
		++i;
	}
//	unsigned char c;
//	while (USART6_STATUS & USART_RXCIF_bm)
//		c = USART6_DATA;
	
	return i;
}

//uint8_t
//xpcc::stm32::BufferedUsart6::flushTransmitBuffer()
//{
//	uint8_t i(0);
//	while(!txBuffer.isEmpty()) {
//		txBuffer.pop();
//		++i;
//	}
//
//	return i;
//}

#endif

