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
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	UART_DEFINES_H
#define	UART_DEFINES_H

// Atmel made a mess with the register names for UART0 between the "old" devices.
// To avoid to much conditional compiling we try to mimic the new names for
// the old devices.
#if defined (UDR) && defined (UDR0)
#undef UDR0
#endif

#if defined (UDR)
#	define UDR0		UDR
#	define UBRR0L	UBRRL
#	define UBRR0H	UBRRH
#	define UCSR0A	UCSRA
#	define UCSR0B	UCSRB
#	define UCSR0C	UCSRC
#	define RXCIE0	RXCIE
#	define TXCIE0	TXCIE
#	define RXEN0	RXEN
#	define TXEN0	TXEN
#	define U2X0		U2X
#	define URSEL0	URSEL
#	define UCSZ00	UCSZ0
#	define UCSZ01	UCSZ1
#	define UDRIE0	UDRIE
#endif

// We want the interrupt names to be "USART0_RX_vect", "USART0_UDRE_vect" and
// "USART0_TX_vect".
#if defined(USART0_RX_vect)
// everything is fine
#elif defined(USART_RX_vect)
#	define USART0_RX_vect		USART_RX_vect
#	define USART0_UDRE_vect		USART_UDRE_vect
#	define USART0_TX_vect		USART_TX_vect
#elif defined(USARTRXC_vect)
#	define USART0_RX_vect		USARTRXC_vect
#	define USART0_UDRE_vect		USARTUDRE_vect
#	define USART0_TX_vect		USARTTXC_vect
#elif defined(USART_RXC_vect)
#	define USART0_RX_vect		USART_RXC_vect
#	define USART0_UDRE_vect		USART_UDRE_vect
#	define USART0_TX_vect		USART_TXC_vect
#endif

#endif	// UART_DEFINES_H

