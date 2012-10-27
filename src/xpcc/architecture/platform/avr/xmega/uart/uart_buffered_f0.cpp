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

#include <xpcc/architecture/platform/avr/xmega.hpp>

#include <xpcc/architecture/driver/atomic/queue.hpp>
#include <xpcc/architecture/driver/atomic/lock.hpp>

#include <xpcc/architecture/driver/gpio.hpp>

#include "uart_f0.hpp"
#include "xpcc_config.hpp"


#ifdef USARTF0_RXC_vect
#if UARTF0_RTS_PIN == -1
// RTS_PIN = -1 deactivates flow control 

namespace
{
	static xpcc::atomic::Queue<uint8_t, UARTF0_RX_BUFFER_SIZE> rxBuffer;
	static xpcc::atomic::Queue<uint8_t, UARTF0_TX_BUFFER_SIZE> txBuffer;
	
	GPIO__INPUT(RXD, F, 2);
	GPIO__OUTPUT(TXD, F, 3);
	
	uint8_t error;
}

// ----------------------------------------------------------------------------
#if UARTF0_RTS_PORT == -1
ISR(USARTF0_RXC_vect)
{		
	// first save the errors
	error |= USARTF0_STATUS & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm);
	// then read the buffer
	uint8_t data = USARTF0_DATA;
	
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
ISR(USARTF0_DRE_vect)
{
	if (txBuffer.isEmpty())
	{
		// transmission finished, disable DRE interrupt
		USARTF0_CTRLA = USART_RXCINTLVL_MED_gc;
	}
	else {
		// get one byte from buffer and write it to the UART buffer
		// which starts the transmission
		USARTF0_DATA = txBuffer.get();
		txBuffer.pop();
	}
}
#endif

// ----------------------------------------------------------------------------
void
xpcc::xmega::BufferedUartF0::initialize()
{
	TXD::set();
	TXD::setOutput();
	
	RXD::setInput();
	
	// interrupts should be disabled during initialisation
	atomic::Lock lock;
		
	// enable receive complete interrupt
	USARTF0.CTRLA = USART_RXCINTLVL_MED_gc;
	
	// setting the frame size to 8 bit
	USARTF0.CTRLC = USART_CHSIZE_8BIT_gc;
	
	// enable both, receiver and transmitter
	USARTF0.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

// ----------------------------------------------------------------------------
void
xpcc::xmega::BufferedUartF0::write(uint8_t c)
{
	uint16_t i(0);		
	while ( !txBuffer.push(c) && (i < 1000) ) {
		++i;
		// wait for a free slot in the buffer
		// but do not wait infinitely
	}
	
	// disable interrupts
	atomic::Lock lock;
	
	// enable DRE interrupt
	USARTF0_CTRLA = USART_RXCINTLVL_MED_gc | USART_DREINTLVL_MED_gc;
}

// ----------------------------------------------------------------------------
void
xpcc::xmega::BufferedUartF0::write(const uint8_t *s, uint8_t n)
{
	while (n-- != 0) {
		BufferedUartF0::write(*s++);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::xmega::BufferedUartF0::read(uint8_t& c)
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
xpcc::xmega::BufferedUartF0::read(uint8_t *buffer, uint8_t n)
{
	uint8_t i(0);
	for (; i < n; ++i)
	{
		if (rxBuffer.isEmpty()) {
			return i;
		}
		else {
			*buffer++ = rxBuffer.get();
			rxBuffer.pop();
		}
	}
	
	return i;
}

uint8_t
xpcc::xmega::BufferedUartF0::readErrorFlags()
{
	return error;
}

void
xpcc::xmega::BufferedUartF0::resetErrorFlags()
{
	error = 0;
}

uint8_t
xpcc::xmega::BufferedUartF0::flushReceiveBuffer()
{
	uint8_t i(0);
	while(!rxBuffer.isEmpty()) {
		rxBuffer.pop();
		++i;
	}
	unsigned char c;
	while (USARTF0_STATUS & USART_RXCIF_bm)
		c = USARTF0_DATA;
	
	return i;
}

//uint8_t
//xpcc::xmega::BufferedUartF0::flushTransmitBuffer()
//{
//	uint8_t i(0);
//	while(!txBuffer.isEmpty()) {
//		txBuffer.pop();
//		++i;
//	}
//
//	return i;
//}

#endif
#endif // USARTF0