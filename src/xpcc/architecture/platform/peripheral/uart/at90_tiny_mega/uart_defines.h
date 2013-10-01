// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AT90_ATTINY_ATMEGA__UART_DEFINES_H
#define	XPCC_AT90_ATTINY_ATMEGA__UART_DEFINES_H

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
#	define DOR0		DOR
#	define FE0		FE
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

#endif	// XPCC_AT90_ATTINY_ATMEGA__UART_DEFINES_H

