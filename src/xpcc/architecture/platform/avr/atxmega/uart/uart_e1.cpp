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

#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/driver/gpio.hpp>

#include "uart_e1.hpp"

#ifdef USARTE1_RXC_vect

namespace
{
	GPIO__INPUT(RXD, E, 6);
	GPIO__OUTPUT(TXD, E, 7);
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::UartE1::setBaudrateRegister(uint16_t ubrr)
{
	TXD::set();
	TXD::setOutput();
	
	RXD::setInput();
	
	// set baud rate
	if (ubrr & 0x8000) {
		USARTE1_CTRLB = USART_CLK2X_bm;  //Enable 2x speed 
		ubrr &= ~0x8000;
	}
	else {
		USARTE1_CTRLB = 0;
	}
	USARTE1_BAUDCTRLB = static_cast<uint8_t>(ubrr >> 8);
	USARTE1_BAUDCTRLA = static_cast<uint8_t>(ubrr);
	
	// setting the frame size to 8 bit
	USARTE1_CTRLC = USART_CHSIZE_8BIT_gc;
	
	// enable both, receiver and transmitter
	USARTE1_CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::UartE1::write(char c)
{
	while (!(USARTE1_STATUS & USART_DREIF_bm))
		;
	
	USARTE1_DATA = c;
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::UartE1::write(const char *s)
{
	char c;
	while ((c = *s++)) {
		UartE1::write(c);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::atxmega::UartE1::read(char& c)
{
	if (USARTE1_STATUS & USART_RXCIF_bm) {
		// the status flag is cleared by reading DATA
		c = USARTE1_DATA;
		return true;
	}
	return false;
}

#endif // USARTE1