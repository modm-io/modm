
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../../hal/atomic/queue.hpp"

#include "uart_defines.h"
#include "uart_defaults.h"

#include "../uart2.hpp"

#ifdef ATMEGA_USART2

static xpcc::atomic::Queue<char, UART2_RX_BUFFER_SIZE> rxBuffer;

// ----------------------------------------------------------------------------
// called when the UART has received a character
//
ISR(UART2_RECEIVE_INTERRUPT)
{
	uint8_t data = UART2_DATA;
	
	// read UART status register and UART data register
	//uint8_t usr  = UART2_STATUS;
	
/*	uint8_t last_rx_error;
	last_rx_error = usr & ((1 << FE2) | (1 << DOR2));*/
	
	// TODO Fehlerbehandlung
	rxBuffer.push(data);
}

// ----------------------------------------------------------------------------
void
xpcc::Uart2::setBaudrateRegister(uint16_t ubrr)
{
	// Set baud rate
	if (ubrr & 0x8000) {
		UART2_STATUS = (1 << U2X2);  //Enable 2x speed 
		ubrr &= ~0x8000;
	}
	else {
		UART2_STATUS = 0;
	}
	UBRR2H = (uint8_t) (ubrr >> 8);
	UBRR2L = (uint8_t)  ubrr;

	// Enable USART receiver and transmitter and receive complete interrupt
	UART2_CONTROL = (1 << RXCIE2) | (1 << RXEN2) | (1 << TXEN2);
	
	// Set frame format: asynchronous, 8data, no parity, 1stop bit
	#ifdef URSEL1
	UCSR2C = (1 << URSEL2) | (3 << UCSZ20);
	#else
	UCSR2C = (3 << UCSZ20);
	#endif
}

// ----------------------------------------------------------------------------
bool
xpcc::Uart2::get(char& c)
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

#endif	// ATMEGA_USART2

