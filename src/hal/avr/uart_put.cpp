
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "../../data_structure/isr_queue.hpp"

#include "uart_defs.h"
#include "uart.hpp"

static xpcc::IsrQueue<char, UART_TX_BUFFER_SIZE> txBuffer;

// ----------------------------------------------------------------------------
// called when the UART is ready to transmit the next byte
//
ISR(UART0_TRANSMIT_INTERRUPT)
{
	if (txBuffer.isEmpty())
	{
		// transmission finished, disable UDRE interrupt
		UART0_CONTROL &= ~(1 << UART0_UDRIE);
	}
	else {
		// get one byte from buffer and write it to UART (starts transmission)
		UART0_DATA = txBuffer.get();
		txBuffer.pop();
	}
}

// ----------------------------------------------------------------------------
void
xpcc::Uart::put(char c)
{
	while (!txBuffer.push(c)) {
		// wait for a free slot in the buffer
	}
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		// enable UDRE interrupt
		UART0_CONTROL |= (1 << UART0_UDRIE);
	}
}
