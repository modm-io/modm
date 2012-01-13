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

#include <xpcc/architecture/platform/avr/atxmega.hpp>
#include <xpcc/architecture/driver/gpio.hpp>

#include "uart_d1.hpp"

#ifdef USARTD1_RXC_vect

namespace
{
	GPIO__INPUT(RXD, D, 6);
	GPIO__OUTPUT(TXD, D, 7);
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::UartD1::initialise()
{
	TXD::set();
	TXD::setOutput();
	
	RXD::setInput();
	
	// setting the frame size to 8 bit
	USARTD1_CTRLC = USART_CHSIZE_8BIT_gc;
	
	// enable both, receiver and transmitter
	USARTD1_CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::UartD1::write(uint8_t c)
{
	while (!(USARTD1_STATUS & USART_DREIF_bm))
		;
	
	USARTD1_DATA = c;
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::UartD1::write(const uint8_t *s, uint8_t n)
{
	while (--n != 0) {
		UartD1::write(*s++);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::atxmega::UartD1::read(uint8_t& c)
{
	if (USARTD1_STATUS & USART_RXCIF_bm) {
		// the status flag is cleared by reading DATA
		c = USARTD1_DATA;
		return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
uint8_t
xpcc::atxmega::UartD1::read(uint8_t *b, uint8_t n)
{
	/*
	 * The delay of 1.5 times frame time could be calculated at compile time
	 * if the baud rate was known. It's only known in the constructor, not here. 
	 * 
	 * This manual calculation assumes a baud rate of 115200. 
	 * On cycle of the loop if(USARTD1_STATUS ...) takes nine instructions:
	 * 
	 * if (USART...)		LDS		2
	 * 						SBRS	2
	 * 						RJMP	2
	 * if (--delay == 0)	SUBI	1
	 * 						BRNE	2
	 * 
	 * Assuming a baud rate of 115200 a frame (1 stop, 8 data, 1 stop bit)
	 * lasts 86.6usec. At a CPU frequency of F_CPU a CPU cycle lasts 
	 * 1/F_CPU seconds. So a frame takes (86.6usec * F_CPU) cycles, or
	 * 86.6 * (F_CPU/1000000) = (F_CPU / 11547) cycles. Adding a margin of
	 * 50% (F_CPU / 7698) cycles are equal to 1.5 times the frame time. 
	 * The loop needs nine instructions so the loop should loop 
	 * (F_CPU / 69282) times.   
	 * 
	 * Example for 32 MHz:
	 * A CPU cycle takes 31.25nsec. In 86.6usec 2771.2 CPU cycles are executed. 
	 * The loop takes nine instructions so it is enough to loop 308 times for 
	 * one frame time.  After adding 50% the loop should loop 462 times. 
	 *
	 * This is the same result as (32000000 / 69282) = 462. 
	 */
	uint8_t rcvd = 0;
	uint16_t const delayStart = (F_CPU / 69282);		// TODO Make depend on baudrate 
	uint16_t delay = delayStart; 
	while (1)
	{
		if (USARTD1_STATUS & USART_RXCIF_bm)
		{
			// data available
			*b++ = USARTD1_DATA;
			if (++rcvd == n) {
				return rcvd;
			}
			delay = delayStart;	
		}
		else {
			// no new data, but wait a little longer
			if (--delay == 0) {
				return rcvd;
			}
		}
	}
}

#endif // USARTD1