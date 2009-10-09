
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../hal/atomic/queue.hpp"

#include "uart_defs.h"
#include "uart.hpp"

static xpcc::atomic::Queue<char, UART_RX_BUFFER_SIZE> rxBuffer;

// ----------------------------------------------------------------------------
// called when the UART has received a character
//
ISR(UART0_RECEIVE_INTERRUPT)
{
	uint8_t data = UART0_DATA;
	
	// read UART status register and UART data register
	//uint8_t usr  = UART0_STATUS;
	
/*	uint8_t last_rx_error;
#if defined(AT90_UART)
	last_rx_error = usr & ((1 << FE) | (1 << DOR));
#elif defined(ATMEGA_USART)
	last_rx_error = usr & ((1 << FE) | (1 << DOR));
#elif defined(ATMEGA_USART0)
	last_rx_error = usr & ((1 << FE0) | (1 << DOR0));
#elif defined (ATMEGA_UART)
	last_rx_error = usr & ((1 << FE) | (1 << DOR));
#endif*/
	
	// TODO Fehlerbehandlung
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
void
xpcc::Uart::setBaudrate(uint16_t baudrate)
{
	
#if defined( AT90_UART )
	
	// set baud rate
	UBRR = (uint8_t) baudrate; 

	// enable UART receiver and transmmitter and receive complete interrupt
	UART0_CONTROL = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
	
#elif defined (ATMEGA_USART)

	// Set baudrate
	if (baudrate & 0x8000)
	{
		UART0_STATUS = (1 << U2X);	// Enable 2x speed 
		baudrate &= ~0x8000;
	}
	else {
		UART0_STATUS = 0;
	}
	UBRRH = (uint8_t) (baudrate >> 8);
	UBRRL = (uint8_t)  baudrate;

	// Enable USART receiver and transmitter and receive complete interrupt
	UART0_CONTROL = (1 << RXCIE) | (1<<RXEN) | (1<<TXEN);
	
	// Set frame format: asynchronous, 8data, no parity, 1stop bit
	#ifdef URSEL
	UCSRC = (1 << URSEL) | (3 << UCSZ0);
	#else
	UCSRC = (3 << UCSZ0);
	#endif 

#elif defined (ATMEGA_USART0 )

	// Set baud rate
	if (baudrate & 0x8000) 
	{
		UART0_STATUS = (1 << U2X0);  //Enable 2x speed 
		baudrate &= ~0x8000;
	}
	else {
		UART0_STATUS = 0;
	}
	UBRR0H = (uint8_t) (baudrate >> 8);
	UBRR0L = (uint8_t)  baudrate;

	// Enable USART receiver and transmitter and receive complete interrupt
	UART0_CONTROL = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	// Set frame format: asynchronous, 8data, no parity, 1stop bit
	#ifdef URSEL0
	UCSR0C = (1 << URSEL0) | (3 << UCSZ00);
	#else
	UCSR0C = (3 << UCSZ00);
	#endif 

#elif defined ( ATMEGA_UART )
	// set baud rate
	if (baudrate & 0x8000) 
	{
		UART0_STATUS = (1 << U2X);  //Enable 2x speed 
		baudrate &= ~0x8000;
	}
	else {
		UART0_STATUS = 0;
	}
	UBRRHI = (uint8_t) (baudrate >> 8);
	UBRR   = (uint8_t)  baudrate;
	
	// Enable UART receiver and transmitter and receive complete interrupt
	UART0_CONTROL = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
#endif
}

// ----------------------------------------------------------------------------
bool
xpcc::Uart::get(char& c)
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
