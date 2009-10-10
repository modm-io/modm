
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../../hal/atomic/queue.hpp"

#include "uart_defines.h"
#include "uart_defaults.h"

#include "../uart3.hpp"

#ifdef ATMEGA_USART3

static xpcc::atomic::Queue<char, UART3_RX_BUFFER_SIZE> rxBuffer;

// ----------------------------------------------------------------------------
// called when the UART has received a character
//
ISR(UART3_RECEIVE_INTERRUPT)
{
	uint8_t data = UART3_DATA;
	
	// read UART status register and UART data register
	//uint8_t usr  = UART3_STATUS;
	
/*	uint8_t last_rx_error;
	last_rx_error = usr & ((1 << FE1) | (1 << DOR1));*/
	
	// TODO Fehlerbehandlung
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
void
xpcc::Uart3::setBaudrateRegister(uint16_t ubrr)
{
	// Set baud rate
	if (ubrr & 0x8000) {
		UART3_STATUS = (1 << U2X3);  //Enable 2x speed 
		ubrr &= ~0x8000;
	}
	else {
		UART3_STATUS = 0;
	}
	UBRR3H = (uint8_t) (ubrr >> 8);
	UBRR3L = (uint8_t)  ubrr;

	// Enable USART receiver and transmitter and receive complete interrupt
	UART3_CONTROL = (1 << RXCIE3) | (1 << RXEN3) | (1 << TXEN3);
	
	// Set frame format: asynchronous, 8data, no parity, 1stop bit
	#ifdef URSEL1
	UCSR1C = (1 << URSEL3) | (3 << UCSZ30);
	#else
	UCSR1C = (3 << UCSZ30);
	#endif
}

// ----------------------------------------------------------------------------
bool
xpcc::Uart3::get(char& c)
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

#endif	// ATMEGA_USART3

