
// coding: utf-8
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------

#ifndef	UART_DEFS_H
#define	UART_DEFS_H

#if		defined(__AVR_ATmega8__)  || defined(__AVR_ATmega16__) ||\
		defined(__AVR_ATmega32__) || defined(__AVR_ATmega8515__) ||\
		defined(__AVR_ATmega8535__) || defined(__AVR_ATmega323__)
	// ATmega with one USART
	#define ATMEGA_USART
	#define UART0_RECEIVE_INTERRUPT   SIG_UART_RECV
	#define UART0_TRANSMIT_INTERRUPT  SIG_UART_DATA
	#define UART0_STATUS   UCSRA
	#define UART0_CONTROL  UCSRB
	#define UART0_DATA     UDR
	#define UART0_UDRIE    UDRIE

#elif defined(__AVR_ATmega163__) 
	// ATmega163 with one UART
	#define ATMEGA_UART
	#define UART0_RECEIVE_INTERRUPT   SIG_UART_RECV
	#define UART0_TRANSMIT_INTERRUPT  SIG_UART_DATA
	#define UART0_STATUS   UCSRA
	#define UART0_CONTROL  UCSRB
	#define UART0_DATA     UDR
	#define UART0_UDRIE    UDRIE

#elif defined(__AVR_ATmega162__)
	// ATmega with two USART
	#define ATMEGA_USART0
	#define UART0_RECEIVE_INTERRUPT   SIG_USART0_RECV
	#define UART0_TRANSMIT_INTERRUPT  SIG_USART0_DATA
	#define UART0_STATUS   UCSR0A
	#define UART0_CONTROL  UCSR0B
	#define UART0_DATA     UDR0
	#define UART0_UDRIE    UDRIE0
	
	#define ATMEGA_USART1
	#define UART1_RECEIVE_INTERRUPT   SIG_USART1_RECV
	#define UART1_TRANSMIT_INTERRUPT  SIG_USART1_DATA
	#define UART1_STATUS   UCSR1A
	#define UART1_CONTROL  UCSR1B
	#define UART1_DATA     UDR1
	#define UART1_UDRIE    UDRIE1

#elif	defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) ||\
		defined(__AVR_AT90CAN128__)
	// ATmega with two USART
	#define ATMEGA_USART0
	#define UART0_TRANSMIT_INTERRUPT  SIG_UART0_DATA
	#define UART0_RECEIVE_INTERRUPT   SIG_UART0_RECV
	#define UART0_STATUS   UCSR0A
	#define UART0_CONTROL  UCSR0B
	#define UART0_DATA     UDR0
	#define UART0_UDRIE    UDRIE0
	
	#define ATMEGA_USART1
	#define UART1_RECEIVE_INTERRUPT   SIG_UART1_RECV
	#define UART1_TRANSMIT_INTERRUPT  SIG_UART1_DATA
	#define UART1_STATUS   UCSR1A
	#define UART1_CONTROL  UCSR1B
	#define UART1_DATA     UDR1
	#define UART1_UDRIE    UDRIE1

#elif	defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__)
	// ATmega with two USART
	#define ATMEGA_USART0
	#define UART0_RECEIVE_INTERRUPT		USART0_RX_vect
	#define UART0_TRANSMIT_INTERRUPT	USART0_UDRE_vect
	#define UART0_STATUS	UCSR0A
	#define UART0_CONTROL	UCSR0B
	#define UART0_DATA		UDR0
	#define UART0_UDRIE		UDRIE0
	
	#define ATMEGA_USART1
	#define UART1_RECEIVE_INTERRUPT		USART1_RX_vect
	#define UART1_TRANSMIT_INTERRUPT	USART1_UDRE_vect
	#define UART1_STATUS	UCSR1A
	#define UART1_CONTROL	UCSR1B
	#define UART1_DATA		UDR1
	#define UART1_UDRIE		UDRIE1

#elif	defined(__AVR_ATmega640__) || defined(__AVR_ATmega1280__) ||\
		defined(__AVR_ATmega2560__)
	// ATmega with four USART
	#define ATMEGA_USART0
	#define UART0_RECEIVE_INTERRUPT		USART0_RX_vect
	#define UART0_TRANSMIT_INTERRUPT	USART0_UDRE_vect
	#define UART0_STATUS	UCSR0A
	#define UART0_CONTROL	UCSR0B
	#define UART0_DATA		UDR0
	#define UART0_UDRIE		UDRIE0
	
	#define ATMEGA_USART1
	#define UART1_RECEIVE_INTERRUPT		USART1_RX_vect
	#define UART1_TRANSMIT_INTERRUPT	USART1_UDRE_vect
	#define UART1_STATUS	UCSR1A
	#define UART1_CONTROL	UCSR1B
	#define UART1_DATA		UDR1
	#define UART1_UDRIE		UDRIE1
	
	#define ATMEGA_USART2
	#define UART2_RECEIVE_INTERRUPT		USART2_RX_vect
	#define UART2_TRANSMIT_INTERRUPT	USART2_UDRE_vect
	#define UART2_STATUS	UCSR2A
	#define UART2_CONTROL	UCSR2B
	#define UART2_DATA		UDR2
	#define UART2_UDRIE		UDRIE2
	
	#define ATMEGA_USART3
	#define UART3_RECEIVE_INTERRUPT		USART3_RX_vect
	#define UART3_TRANSMIT_INTERRUPT	USART3_UDRE_vect
	#define UART3_STATUS	UCSR3A
	#define UART3_CONTROL	UCSR3B
	#define UART3_DATA		UDR3
	#define UART3_UDRIE		UDRIE3

#elif defined(__AVR_ATmega161__)
	// ATmega with UART
	#error "AVR ATmega161 currently not supported by this library!"

#elif defined(__AVR_ATmega169__) 
	// ATmega with one USART
	#define ATMEGA_USART
	#define UART0_RECEIVE_INTERRUPT   SIG_USART_RECV
	#define UART0_TRANSMIT_INTERRUPT  SIG_USART_DATA
	#define UART0_STATUS   UCSRA
	#define UART0_CONTROL  UCSRB
	#define UART0_DATA     UDR
	#define UART0_UDRIE    UDRIE

#elif defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) || \
	  defined(__AVR_ATmega168__)
	#define ATMEGA_USART0
	#define UART0_RECEIVE_INTERRUPT   SIG_USART_RECV
	#define UART0_TRANSMIT_INTERRUPT  SIG_USART_DATA
	#define UART0_STATUS   UCSR0A
	#define UART0_CONTROL  UCSR0B
	#define UART0_DATA     UDR0
	#define UART0_UDRIE    UDRIE0

#elif defined(__AVR_ATtiny2313__)
	#define ATMEGA_USART
	#define UART0_RECEIVE_INTERRUPT   SIG_USART0_RX 
	#define UART0_TRANSMIT_INTERRUPT  SIG_USART0_UDRE
	#define UART0_STATUS   UCSRA
	#define UART0_CONTROL  UCSRB
	#define UART0_DATA     UDR
	#define UART0_UDRIE    UDRIE

#elif defined(__AVR_ATmega644__)
	#define ATMEGA_USART0
	#define UART0_RECEIVE_INTERRUPT   USART0_RX_vect
	#define UART0_TRANSMIT_INTERRUPT  USART0_UDRE_vect
	#define UART0_STATUS   UCSR0A
	#define UART0_CONTROL  UCSR0B
	#define UART0_DATA     UDR0
	#define UART0_UDRIE    UDRIE0

#else
	// TODO
	#warning "no UART definition for MCU available"
#endif

#endif	// UART_DEFS_H
