// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#include <avr/io.h>
#include <avr/interrupt.h>

#include <xpcc/architecture/driver/atomic/queue.hpp>
#include <xpcc/architecture/driver/atomic/lock.hpp>

#include "uart_defines.h"
#include "xpcc_config.hpp"

#if defined USART2_RX_vect

#include "uart2.hpp"

static xpcc::atomic::Queue<char, UART2_RX_BUFFER_SIZE> rxBuffer;
static uint8_t error;

// ----------------------------------------------------------------------------
// called when the UART has received a character
// 
ISR(USART2_RX_vect)
{
// TODO: Fix this for all ATmega's
#if defined (DOR2) && defined (FE2)
	error |= UCSR2A & ((1 << FE2) | (1 << DOR2));
#endif
	uint8_t data = UDR2;
	
	// TODO Fehlerbehandlung
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
void
xpcc::atmega::BufferedUart2::setBaudrateRegister(uint16_t ubrr)
{
	// Set baud rate
	if (ubrr & 0x8000) {
		UCSR2A = (1 << U2X2);  //Enable 2x speed 
		ubrr &= ~0x8000;
	}
	else {
		UCSR2A = 0;
	}
	UBRR2L = (uint8_t)  ubrr;
	UBRR2H = (uint8_t) (ubrr >> 8);
	
	// Enable USART receiver and transmitter and receive complete interrupt
	UCSR2B = (1 << RXCIE2) | (1 << RXEN2) | (1 << TXEN2);
	
	// Set frame format: asynchronous, 8data, no parity, 1stop bit
#ifdef URSEL2
    UCSR2C = (1 << URSEL2) | (1 << UCSZ21) | (1 << UCSZ20);
#else
    UCSR2C = (1 << UCSZ21) | (1 << UCSZ20);
#endif
}

// ----------------------------------------------------------------------------
bool
xpcc::atmega::BufferedUart2::read(char& c)
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
xpcc::atmega::BufferedUart2::read(char *buffer, uint8_t n)
{
	for (uint8_t i = 0; i < n; ++i)
	{
		if (rxBuffer.isEmpty()) {
			return n;
		}
		else {
			*buffer++ = rxBuffer.get();
			rxBuffer.pop();
		}
	}
	
	return n;
}

uint8_t
xpcc::atmega::BufferedUart2::readError()
{
	return error;
}

void
xpcc::atmega::BufferedUart2::resetError()
{
	error = 0;
}

uint8_t
xpcc::atmega::BufferedUart2::flushReceiveBuffer()
{
	uint8_t i(0);
	while(!rxBuffer.isEmpty()) {
		rxBuffer.pop();
		++i;
	}
#if defined (RXC2)
	unsigned char c;
	while (UCSR2A & (1 << RXC2))
		c = UDR2;
#endif
	
	return i;
}

#endif
