
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../data_structure/queue.hpp"

#include "uart_defs.h"
#include "uart.hpp"

static xpcc::BoundedQueue<char, UART_TX_BUFFER_SIZE, uint8_t> txBuffer;

// ----------------------------------------------------------------------------
// called when the UART is ready to transmit the next byte
//
ISR(UART0_TRANSMIT_INTERRUPT)
{
	if (!txBuffer.isEmpty())
	{
		// get one byte from buffer and write it to UART (starts transmission)
		UART0_DATA = txBuffer.get();
		txBuffer.pop();
	}
	else {
		// transmission finished, disable UDRE interrupt
		UART0_CONTROL &= ~(1 << UART0_UDRIE);
	}
}

// ----------------------------------------------------------------------------
void
xpcc::Uart::put(char c)
{
	while (!txBuffer.push(c)) {
		// TODO Nicht schön!
		asm volatile ("":::"memory");
	}
	
	// enable UDRE interrupt
	UART0_CONTROL |= (1 << UART0_UDRIE);
}
