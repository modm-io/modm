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

#if defined USART2_UDRE_vect

#include "uart2.hpp"

static xpcc::atomic::Queue<uint8_t, UART2_TX_BUFFER_SIZE> txBuffer;

// ----------------------------------------------------------------------------
// called when the UART is ready to transmit the next byte
// 
ISR(USART2_UDRE_vect)
{
	if (txBuffer.isEmpty())
	{
		// transmission finished, disable UDRE interrupt
		UCSR2B &= ~(1 << UDRIE2);
	}
	else {
		// get one byte from buffer and write it to the UART buffer
		// which starts the transmission
		UDR2 = txBuffer.get();
		txBuffer.pop();
	}
}

// MARK: - write blocking
void
xpcc::atmega::Uart2::writeBlocking(uint8_t data)
{
	// wait until there is some place in the buffer
	while (!write(data))
		;
	
	// wait until everything has been sent
	while (!isWriteFinished())
		;
}

void
xpcc::atmega::Uart2::writeBlocking(const uint8_t *data, std::size_t length)
{
	// first push everything into the buffer
	for (std::size_t i = 0; i < length; ++i)
	{
		while (!write(*data++))
			;
	}
	
	// then wait
	while (!isWriteFinished())
		;
}

void
xpcc::atmega::Uart2::flushWriteBuffer()
{
	// just wait until the last byte has been sent
	while (!isWriteFinished())
		;
}

// MARK: - write
bool
xpcc::atmega::Uart2::write(uint8_t data)
{
	if (!txBuffer.push(data))
		return false;
	
	atomic::Lock lock;
	
	// enable UDRE interrupt
	UCSR2B |= (1 << UDRIE2);
	
	return true;
}

std::size_t
xpcc::atmega::Uart2::write(const uint8_t *data, std::size_t length)
{
	for (std::size_t i = 0; i < length; ++i)
	{
		if (!write(*data++))
		{
			return i;
		}
	}
	
	return length;
}

bool
xpcc::atmega::Uart2::isWriteFinished()
{
	return (txBuffer.isEmpty() && !(UCSR2B & (1 << UDRIE2)));
}

// MARK: - discard
std::size_t
xpcc::atmega::Uart2::discardTransmitBuffer()
{
	{
		atomic::Lock lock;
		UCSR2B &= ~(1 << UDRIE2);
	}
	
	std::size_t i = 0;
	while(!txBuffer.isEmpty())
	{
		txBuffer.pop();
		++i;
	}

	return i;
}

#endif
