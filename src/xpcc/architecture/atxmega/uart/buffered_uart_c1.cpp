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


#include <avr/io.h>
#include <avr/interrupt.h>

#include <xpcc/architecture/general/atomic/queue.hpp>
#include <xpcc/architecture/general/atomic/lock.hpp>

#include <xpcc/architecture/general/gpio.hpp>

#include "buffered_uart_c1.hpp"
#include "xpcc_config.h"

#ifdef USARTC1

namespace
{
	static xpcc::atomic::Queue<char, UARTC1_RX_BUFFER_SIZE> rxBuffer;
	static xpcc::atomic::Queue<char, UARTC1_TX_BUFFER_SIZE> txBuffer;
	
	GPIO__INPUT(RXD, C, 6);
	GPIO__OUTPUT(TXD, C, 7);
}

// ----------------------------------------------------------------------------
ISR(USARTC1_RXC_vect)
{
	uint8_t data = USARTC1_DATA;
	
	// TODO Fehlerbehandlung
	//USARTC1_STATUS & (FERR, BUFOVF, PERR)
	
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
ISR(USARTC1_DRE_vect)
{
	if (txBuffer.isEmpty())
	{
		// transmission finished, disable DRE interrupt
		USARTC1_CTRLA = USART_RXCINTLVL_MED_gc;
	}
	else {
		// get one byte from buffer and write it to the UART buffer
		// which starts the transmission
		USARTC1_DATA = txBuffer.get();
		txBuffer.pop();
	}
}

// ----------------------------------------------------------------------------
void
xpcc::BufferedUartC1::setBaudrateRegister(uint16_t ubrr, bool doubleSpeed)
{
	TXD::high();
	TXD::setOutput();
	
	RXD::setInput();
	
	// interrupts should be disabled during initialization
	atomic::Lock lock;
	
	// set baud rate
	if (doubleSpeed) {
		USARTC1_CTRLB = USART_CLK2X_bm;
	}
	else {
		USARTC1_CTRLB = 0;
	}
	USARTC1_BAUDCTRLB = static_cast<uint8_t>(ubrr >> 8);
	USARTC1_BAUDCTRLA = static_cast<uint8_t>(ubrr);
	
	// enable receive complete interrupt
	USARTC1_CTRLA = USART_RXCINTLVL_MED_gc;
	
	// setting the frame size to 8 bit
	USARTC1_CTRLC = USART_CHSIZE_8BIT_gc;
	
	// enable both, receiver and transmitter
	USARTC1_CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

// ----------------------------------------------------------------------------
void
xpcc::BufferedUartC1::write(char c)
{
	while (!txBuffer.push(c)) {
		// wait for a free slot in the buffer
	}
	
	// disable interrupts
	atomic::Lock lock;
	
	// enable DRE interrupt
	USARTC1_CTRLA = USART_RXCINTLVL_MED_gc | USART_DREINTLVL_MED_gc;
}

// ----------------------------------------------------------------------------
void
xpcc::BufferedUartC1::write(const char *s)
{
	char c;
	while ((c = *s++)) {
		BufferedUartC1::write(c);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::BufferedUartC1::read(char& c)
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
xpcc::BufferedUartC1::read(char *buffer, uint8_t n)
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

#endif // USARTC1