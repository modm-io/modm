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

#if defined USART3_RX_vect

#include "uart3.hpp"

static xpcc::atomic::Queue<char, UART3_RX_BUFFER_SIZE> rxBuffer;
static uint8_t error;

// ----------------------------------------------------------------------------
// called when the UART has received a character
// 
ISR(USART3_RX_vect)
{
	// read error flags
	error |= UCSR3A & ((1 << FE3) | (1 << DOR3));
	
	uint8_t data = UDR3;
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
void
xpcc::atmega::BufferedUart3::setBaudrateRegister(uint16_t ubrr)
{
	// Set baud rate
	if (ubrr & 0x8000) {
		UCSR3A = (1 << U2X3);  //Enable 2x speed 
		ubrr &= ~0x8000;
	}
	else {
		UCSR3A = 0;
	}
	UBRR3L = (uint8_t)  ubrr;
	UBRR3H = (uint8_t) (ubrr >> 8);
	
	// Enable USART receiver and transmitter and receive complete interrupt
	UCSR3B = (1 << RXCIE3) | (1 << RXEN3) | (1 << TXEN3);
	
	// Set frame format: asynchronous, 8data, no parity, 1stop bit
#ifdef URSEL3
    UCSR3C = (1 << URSEL3) | (1 << UCSZ31) | (1 << UCSZ30);
#else
    UCSR3C = (1 << UCSZ31) | (1 << UCSZ30);
#endif
}

// ----------------------------------------------------------------------------
bool
xpcc::atmega::BufferedUart3::read(char& c)
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
xpcc::atmega::BufferedUart3::read(char *buffer, uint8_t n)
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
xpcc::atmega::BufferedUart3::readError()
{
	return error;
}

void
xpcc::atmega::BufferedUart3::resetError()
{
	error = 0;
}

uint8_t
xpcc::atmega::BufferedUart3::flushReceiveBuffer()
{
	uint8_t i(0);
	while(!rxBuffer.isEmpty()) {
		rxBuffer.pop();
		++i;
	}
	
#if defined (RXC3)
	unsigned char c;
	while (UCSR3A & (1 << RXC3))
		c = UDR3;
#endif
	
	return i;
}

#endif
