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

#include <xpcc/architecture/platform/avr/atxmega.hpp>

#include <xpcc/architecture/driver/atomic/queue.hpp>
#include <xpcc/architecture/driver/atomic/lock.hpp>

#include <xpcc/architecture/driver/gpio.hpp>

#include "uart_d1.hpp"
#include "xpcc_config.hpp"

#ifdef USARTD1_RXC_vect

namespace
{
	static xpcc::atomic::Queue<uint8_t, UARTD1_RX_BUFFER_SIZE> rxBuffer;
	static xpcc::atomic::Queue<uint8_t, UARTD1_TX_BUFFER_SIZE> txBuffer;
	
	GPIO__INPUT(RXD, D, 6);
	GPIO__OUTPUT(TXD, D, 7);
	
	uint8_t error;
}

// ----------------------------------------------------------------------------
ISR(USARTD1_RXC_vect)
{	
	// first save the errors
	error |= USARTD1_STATUS & (USART_FERR_bm | USART_BUFOVF_bm | USART_PERR_bm);
	// then read the buffer
	uint8_t data = USARTD1_DATA;
	
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
ISR(USARTD1_DRE_vect)
{
	if (txBuffer.isEmpty())
	{
		// transmission finished, disable DRE interrupt
		USARTD1_CTRLA = USART_RXCINTLVL_MED_gc;
	}
	else {
		// get one byte from buffer and write it to the UART buffer
		// which starts the transmission
		USARTD1_DATA = txBuffer.get();
		txBuffer.pop();
	}
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::BufferedUartD1::setBaudrateRegister(uint16_t ubrr, bool doubleSpeed)
{
	TXD::set();
	TXD::setOutput();
	
	RXD::setInput();
	
	// interrupts should be disabled during initialization
	atomic::Lock lock;
	
	// set baud rate
	if (doubleSpeed) {
		USARTD1.CTRLB = USART_CLK2X_bm;
	}
	else {
		USARTD1.CTRLB = 0;
	}
	USARTD1.BAUDCTRLB = static_cast<uint8_t>(ubrr >> 8);
	USARTD1.BAUDCTRLA = static_cast<uint8_t>(ubrr);
	
	// enable receive complete interrupt
	USARTD1.CTRLA = USART_RXCINTLVL_MED_gc;
	
	// setting the frame size to 8 bit
	USARTD1.CTRLC = USART_CHSIZE_8BIT_gc;
	
	// enable both, receiver and transmitter
	USARTD1.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::BufferedUartD1::write(uint8_t c)
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
	USARTD1_CTRLA = USART_RXCINTLVL_MED_gc | USART_DREINTLVL_MED_gc;
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::BufferedUartD1::write(const uint8_t *s, uint8_t n)
{
	while (--n != 0) {
		BufferedUartD1::write(*s++);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::atxmega::BufferedUartD1::read(uint8_t& c)
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
xpcc::atxmega::BufferedUartD1::read(uint8_t *buffer, uint8_t n)
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
xpcc::atxmega::BufferedUartD1::readErrorFlags()
{
	return error;
}

void
xpcc::atxmega::BufferedUartD1::resetErrorFlags()
{
	error = 0;
}

uint8_t
xpcc::atxmega::BufferedUartD1::flushReceiveBuffer()
{
	uint8_t i(0);
	while(!rxBuffer.isEmpty()) {
		rxBuffer.pop();
		++i;
	}
	unsigned char c;
	while (USARTD1_STATUS & USART_RXCIF_bm)
		c = USARTD1_DATA;
	
	return i;
}

//uint8_t
//xpcc::atxmega::BufferedUartD1::flushTransmitBuffer()
//{
//	uint8_t i(0);
//	while(!txBuffer.isEmpty()) {
//		txBuffer.pop();
//		++i;
//	}
//
//	return i;
//}


#endif // USARTD1