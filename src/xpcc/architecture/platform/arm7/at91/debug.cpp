// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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
 * $Id: debug.cpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#include "../at91.hpp"
#include "debug.hpp"

namespace
{
#ifdef __ARM_AT91SAM7S__
	GPIO__INPUT(RXD, A, 27);
	GPIO__OUTPUT(TXD, A, 28);
#else
	GPIO__INPUT(RXD, A, 5);
	GPIO__OUTPUT(TXD, A, 6);
#endif
}

// ----------------------------------------------------------------------------
void
xpcc::at91::Debug::setBaudrate(uint32_t baudrate)
{
	//uint16_t ubrr;
	//ubrr = (F_CPU / (baudrate * 16l)) - 1;
	//setBaudrateRegister(ubrr);
	
	// Open PIO for DBGU
	AT91F_DBGU_CfgPIO();
	
	// Configure DBGU
	AT91F_US_Configure (
		(AT91PS_USART) AT91C_BASE_DBGU,
		F_CPU,
		AT91C_US_ASYNC_MODE,
		baudrate,
		0);
	
	// Enable Transmitter & receivier
	((AT91PS_USART) AT91C_BASE_DBGU)->US_CR = AT91C_US_RXEN | AT91C_US_TXEN;
}

// ----------------------------------------------------------------------------
void
xpcc::at91::Debug::write(char data)
{
	while (!AT91F_US_TxReady((AT91PS_USART)AT91C_BASE_DBGU)) {
		// wait until the buffer is free
	}
	AT91F_US_PutChar((AT91PS_USART) AT91C_BASE_DBGU, data);
}

void
xpcc::at91::Debug::write(const char *string)
{
	char c;
	while((c = *string++) != '\0') {
		write(c);
	}
}

// ----------------------------------------------------------------------------
bool
xpcc::at91::Debug::read(char& c)
{
	if (AT91F_US_RxReady((AT91PS_USART) AT91C_BASE_DBGU))
	{
		c = AT91F_US_GetChar((AT91PS_USART) AT91C_BASE_DBGU);
		return true;
	}
	
	return false;
}
