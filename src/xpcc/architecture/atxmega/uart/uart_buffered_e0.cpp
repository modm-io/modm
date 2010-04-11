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

#include "uart_e0.hpp"
#include "xpcc_config.h"

#ifdef USARTE0

namespace
{
	static xpcc::atomic::Queue<char, UARTE0_RX_BUFFER_SIZE> rxBuffer;
	static xpcc::atomic::Queue<char, UARTE0_TX_BUFFER_SIZE> txBuffer;
	
	GPIO__INPUT(RXD, E, 2);
	GPIO__OUTPUT(TXD, E, 3);
}

// ----------------------------------------------------------------------------
ISR(USARTE0_RXC_vect)
{
	uint8_t data = USARTE0_DATA;
	
	// TODO Fehlerbehandlung
	//USARTE0_STATUS & (FERR, BUFOVF, PERR)
	
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
ISR(USARTE0_DRE_vect)
{
	if (txBuffer.isEmpty())
	{
		// transmission finished, disable DRE interrupt
		USARTE0_CTRLA = USART_RXCINTLVL_MED_gc;
	}
	else {
		// get one byte from buffer and write it to the UART buffer
		// which starts the transmission
		USARTE0_DATA = txBuffer.get();
		txBuffer.pop();
	}
}

// ----------------------------------------------------------------------------
void
xpcc::BufferedUartE0::setBaudrateRegister(uint16_t ubrr, bool doubleSpeed)
{
	TXD::high();
	TXD::setOutput();
	
	RXD::setInput();
	
	// interrupts should be disabled during initialization
	atomic::Lock lock;
	
	// set baud rate
	if (doubleSpeed) {
		USARTE0.CTRLB = USART_CLK2X_bm;
	}
	else {
		USARTE0.CTRLB = 0;
	}
	USARTE0.BAUDCTRLB = static_cast<uint8_t>(ubrr >> 8);
	USARTE0.BAUDCTRLA = static_cast<uint8_t>(ubrr);
	
	// enable receive complete interrupt
	USARTE0.CTRLA = USART_RXCINTLVL_MED_gc;
	
	// setting the frame size to 8 bit
	USARTE0.CTRLC = USART_CHSIZE_8BIT_gc;
	
	// enable both, receiver and transmitter
	USARTE0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

// ----------------------------------------------------------------------------
void
xpcc::BufferedUartE0::write(char c)
{
	uint16_t i(0);		
	while ( !txBuffer.push(c) && (i < 1000) ) {
		++i;
		// wait for a free slot in the buffer
		// but do not wait infinity
	}
	
	// disable interrupts
	atomic::Lock lock;
	
	// enable DRE interrupt
	USARTE0_CTRLA = USART_RXCINTLVL_MED_gc | USART_DREINTLVL_MED_gc;
}

// ----------------------------------------------------------------------------
void
xpcc::BufferedUartE0::write(const char *s)
{
	char c;
	while ((c = *s++)) {
		BufferedUartE0::write(c);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::BufferedUartE0::read(char& c)
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
xpcc::BufferedUartE0::read(char *buffer, uint8_t n)
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

#endif // USARTE0