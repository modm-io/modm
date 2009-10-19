
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../../hal/atomic/queue.hpp"
#include "../../../hal/atomic/lock.hpp"

#include "uart_defines.h"
#include "uart_defaults.h"

#include "../uart0.hpp"

#ifndef __AVR_ATxmega128A1__

static xpcc::atomic::Queue<char, UART0_TX_BUFFER_SIZE> txBuffer;

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
xpcc::Uart0::put(char c)
{
	while (!txBuffer.push(c)) {
		// wait for a free slot in the buffer
	}
	
	atomic::Lock lock;
	
	// enable UDRE interrupt
	UART0_CONTROL |= (1 << UART0_UDRIE);
}

#else
	#warning	TODO!
#endif
