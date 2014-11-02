// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AT90_ATTINY_ATMEGA_UART_DEFINES_H
#define	XPCC_AT90_ATTINY_ATMEGA_UART_DEFINES_H

// Atmel made a mess with the register names for UART0 between the "old" devices.
// To avoid to much conditional compiling we try to mimic the new names for
// the old devices.

#if defined (UDR) && not defined (UDR0)
#	define UDR0		UDR
#endif
#if defined (UBRRL) && not defined (UBRR0L)
#	define UBRR0L	UBRRL
#endif
#if defined (UBRRH) && not defined (UBRR0H)
#	define UBRR0H	UBRRH
#endif
#if defined (UCSRA) && not defined (UCSR0A)
#	define UCSR0A	UCSRA
#endif
#if defined (UCSRB) && not defined (UCSR0B)
#	define UCSR0B	UCSRB
#endif
#if defined (UCSRC) && not defined (UCSR0C)
#	define UCSR0C	UCSRC
#endif
#if defined (DOR) && not defined (DOR0)
#	define DOR0		DOR
#endif
#if defined (FE) && not defined (FE0)
#	define FE0		FE
#endif
#if defined (RXCIE) && not defined (RXCIE0)
#	define RXCIE0	RXCIE
#endif
#if defined (TXCIE) && not defined (TXCIE0)
#	define TXCIE0	TXCIE
#endif
#if defined (RXEN) && not defined (RXEN0)
#	define RXEN0	RXEN
#endif
#if defined (TXEN) && not defined (TXEN0)
#	define TXEN0	TXEN
#endif
#if defined (U2X) && not defined (U2X0)
#	define U2X0		U2X
#endif
#if defined (URSEL) && not defined (URSEL0)
#	define URSEL0	URSEL
#endif
#if defined (UCSZ0) && not defined (UCSZ00)
#	define UCSZ00	UCSZ0
#endif
#if defined (UCSZ1) && not defined (UCSZ01)
#	define UCSZ01	UCSZ1
#endif
#if defined (UDRIE) && not defined (UDRIE0)
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

#endif	// XPCC_AT90_ATTINY_ATMEGA_UART_DEFINES_H

