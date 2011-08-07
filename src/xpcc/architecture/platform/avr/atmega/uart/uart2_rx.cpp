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

#ifdef ATMEGA_HAS_UART2

#include "uart2.hpp"

static xpcc::atomic::Queue<char, UART2_RX_BUFFER_SIZE> rxBuffer;

// ----------------------------------------------------------------------------
// called when the UART has received a character
// 
ISR(UART2_RECEIVE_INTERRUPT)
{
	uint8_t data = UART2_DATA;
	
	// read UART status register and UART data register
	//uint8_t usr  = UART2_STATUS;
	
//	uint8_t last_rx_error;
//#if defined(ATMEGA_USART)
//	last_rx_error = usr & ((1 << FE) | (1 << DOR));
//#elif defined(ATMEGA_USART2)
//	last_rx_error = usr & ((1 << FE2) | (1 << DOR2));
//#elif defined (ATMEGA_UART)
//	last_rx_error = usr & ((1 << FE) | (1 << DOR));
//#endif
	
	// TODO Fehlerbehandlung
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
void
xpcc::atmega::BufferedUart2::setBaudrateRegister(uint16_t ubrr)
{

	// Set baud rate
	if (ubrr & 0x8000) {
		UART2_STATUS = (1 << U2X2);  //Enable 2x speed 
		ubrr &= ~0x8000;
	}
	else {
		UART2_STATUS = 0;
	}
	UBRR2H = (uint8_t) (ubrr >> 8);
	UBRR2L = (uint8_t)  ubrr;

	// Enable USART receiver and transmitter and receive complete interrupt
	UART2_CONTROL = (1 << RXCIE2) | (1 << RXEN2) | (1 << TXEN2);
	
	// Set frame format: asynchronous, 8data, no parity, 1stop bit
	#ifdef URSEL2
	UCSR2C = (1 << URSEL2) | (3 << UCSZ20);
	#else
	UCSR2C = (3 << UCSZ20);
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

#endif
