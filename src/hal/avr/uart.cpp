
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart_defs.h"
#include "uart.hpp"

// ----------------------------------------------------------------------------
// called when the UART has received a character
//
ISR(UART0_RECEIVE_INTERRUPT)
{
	uint8_t data;
	//uint8_t usr;
	//uint8_t last_rx_error;
	
	// read UART status register and UART data register
	//usr  = UART0_STATUS;
	data = UART0_DATA;
	
/*#if defined(AT90_UART)
	last_rx_error = usr & ((1 << FE) | (1 << DOR));
#elif defined(ATMEGA_USART)
	last_rx_error = usr & ((1 << FE) | (1 << DOR));
#elif defined(ATMEGA_USART0)
	last_rx_error = usr & ((1 << FE0) | (1 << DOR0));
#elif defined (ATMEGA_UART)
	last_rx_error = usr & ((1 << FE) | (1 << DOR));
#endif*/
	
	// calculate buffer index
	uint8_t tmphead = uart_rx_head + 1;
	if (tmphead >= UART_RX_BUFFER_SIZE) {
		tmphead = 0;
	}
	
	if (tmphead == uart_rx_tail) {
		// error: receive buffer overflow
		//last_rx_error = UART_BUFFER_OVERFLOW;
	}
	else {
		// store new index and received data
		uart_rx_head = tmphead;
		uart_rx_buffer[tmphead] = data;
	}
	
	//uart_last_rx_error = last_rx_error;
}

// ----------------------------------------------------------------------------
void
xpcc::Uart::setBitrate(uint16_t baudrate)
{
	uart_tx_head = 0;
	uart_tx_tail = 0;
	uart_rx_head = 0;
	uart_rx_tail = 0;
	
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
	uint8_t tmptail;
	
	if (uart_rx_head == uart_rx_tail) {
		return false;
	}
	
	// calculate and store new buffer index
	tmptail = uart_rx_tail + 1;
	if (tmptail >= UART_RX_BUFFER_SIZE) {
		tmptail = 0;
	}
	uart_rx_tail = tmptail;
	
	// get data from receive buffer
	c = uart_rx_buffer[tmptail];
	
	return true;
}
