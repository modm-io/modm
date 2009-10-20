
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../../hal/atomic/queue.hpp"

#include "uart_defines.h"
#include "uart_defaults.h"

#include "../uart1.hpp"

#ifdef ATMEGA_USART1

static xpcc::atomic::Queue<char, UART1_RX_BUFFER_SIZE> rxBuffer;

// ----------------------------------------------------------------------------
// called when the UART has received a character
//
ISR(UART1_RECEIVE_INTERRUPT)
{
	uint8_t data = UART1_DATA;
	
	// read UART status register and UART data register
	//uint8_t usr  = UART1_STATUS;
	
/*	uint8_t last_rx_error;
	last_rx_error = usr & ((1 << FE1) | (1 << DOR1));*/
	
	// TODO Fehlerbehandlung
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
void
xpcc::Uart1::setBaudrateRegister(uint16_t ubrr)
{
	// Set baud rate
	if (ubrr & 0x8000) {
		UART1_STATUS = (1 << U2X1);  //Enable 2x speed 
		ubrr &= ~0x8000;
	}
	else {
		UART1_STATUS = 0;
	}
	UBRR1H = (uint8_t) (ubrr >> 8);
	UBRR1L = (uint8_t)  ubrr;

	// Enable USART receiver and transmitter and receive complete interrupt
	UART1_CONTROL = (1 << RXCIE1) | (1 << RXEN1) | (1 << TXEN1);
	
	// Set frame format: asynchronous, 8data, no parity, 1stop bit
	#ifdef URSEL1
	UCSR1C = (1 << URSEL1) | (3 << UCSZ10);
	#else
	UCSR1C = (3 << UCSZ10);
	#endif
}

// ----------------------------------------------------------------------------
bool
xpcc::Uart1::get(char& c)
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

#endif	// ATMEGA_USART1

