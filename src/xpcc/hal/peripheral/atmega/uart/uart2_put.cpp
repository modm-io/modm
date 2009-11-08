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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.tmpl file instead and re-run the
 * script 'generate.py'.
 *
 * Generated 08 Nov 2009, 18:08:18
 */
// ----------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>

#include <xpcc/hal/atomic/queue.hpp>
#include <xpcc/hal/atomic/lock.hpp>

#include "uart_defines.h"
#include "uart_defaults.h"

#include "uart2.hpp"

static xpcc::atomic::Queue<char, UART2_TX_BUFFER_SIZE> txBuffer;

// ----------------------------------------------------------------------------
// called when the UART is ready to transmit the next byte

ISR(UART2_TRANSMIT_INTERRUPT)
{
	if (txBuffer.isEmpty())
	{
		// transmission finished, disable UDRE interrupt
		UART2_CONTROL &= ~(1 << UART2_UDRIE);
	}
	else {
		// get one byte from buffer and write it to UART (starts transmission)
		UART2_DATA = txBuffer.get();
		txBuffer.pop();
	}
}

// ----------------------------------------------------------------------------
void
xpcc::Uart2::put(char c)
{
	while (!txBuffer.push(c)) {
		// wait for a free slot in the buffer
	}
	
	atomic::Lock lock;
	
	// enable UDRE interrupt
	UART2_CONTROL |= (1 << UART2_UDRIE);
}
