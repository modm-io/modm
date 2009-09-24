
#include <avr/io.h>
#include <avr/interrupt.h>

#include "uart_defs.h"
#include "uart.hpp"

// ----------------------------------------------------------------------------
// called when the UART is ready to transmit the next byte
//
ISR(UART0_TRANSMIT_INTERRUPT)
{
	if (uart_tx_head != uart_tx_tail)
	{
		// calculate and store new buffer index
		uint8_t tmptail = uart_tx_tail + 1;
		if (tmptail >= UART_TX_BUFFER_SIZE) {
			tmptail = 0;
		}
		uart_tx_tail = tmptail;
		
		// get one byte from buffer and write it to UART (starts transmission)
		UART0_DATA = uart_tx_buffer[tmptail];
	}
	else {
		// tx buffer empty, disable UDRE interrupt
		UART0_CONTROL &= ~(1 << UART0_UDRIE);
	}
}

// ----------------------------------------------------------------------------
void
xpcc::Uart::put(char c)
{
	uint8_t tmphead;

	tmphead = uart_tx_head + 1;
	if (tmphead >= UART_TX_BUFFER_SIZE) {
		tmphead = 0;
	}
	
	while (tmphead == uart_tx_tail) {
		// wait for free space in buffer
	}
	
	uart_tx_buffer[tmphead] = c;
	uart_tx_head = tmphead;

	// enable UDRE interrupt
	UART0_CONTROL |= (1 << UART0_UDRIE);
}
