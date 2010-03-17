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

#include <xpcc/architecture/general/gpio.hpp>

#include "uart_f0.hpp"

#ifdef USARTF0

namespace {
	GPIO__INPUT(RXD, F, 2);
	GPIO__OUTPUT(TXD, F, 3);
}

// ----------------------------------------------------------------------------
void
xpcc::UartF0::setBaudrateRegister(uint16_t ubrr)
{
	TXD::high();
	TXD::setOutput();
	
	RXD::setInput();
	
	// set baud rate
	if (ubrr & 0x8000) {
		USARTF0_CTRLB = USART_CLK2X_bm;  //Enable 2x speed 
		ubrr &= ~0x8000;
	}
	else {
		USARTF0_CTRLB = 0;
	}
	USARTF0_BAUDCTRLB = static_cast<uint8_t>(ubrr >> 8);
	USARTF0_BAUDCTRLA = static_cast<uint8_t>(ubrr);
	
	// setting the frame size to 8 bit
	USARTF0_CTRLC = USART_CHSIZE_8BIT_gc;
	
	// enable both, receiver and transmitter
	USARTF0_CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

// ----------------------------------------------------------------------------
void
xpcc::UartF0::write(char c)
{
	while (!(USARTF0_STATUS & USART_DREIF_bm))
		;
	
	USARTF0_DATA = c;
}

// ----------------------------------------------------------------------------
void
xpcc::UartF0::write(const char *s)
{
	char c;
	while ((c = *s++)) {
		UartF0::write(c);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::UartF0::read(char& c)
{
	if (USARTF0_STATUS & USART_RXCIF_bm) {
		// the status flag is cleared by reading DATA
		c = USARTF0_DATA;
		return true;
	}
	return false;
}

#endif // USARTF0