
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../../hal/atomic/queue.hpp"
#include "../../../hal/atomic/lock.hpp"

#include "uart_defines.h"
#include "uart_defaults.h"

#include "../uart3.hpp"

#ifdef ATMEGA_USART3

static xpcc::atomic::Queue<char, UART3_TX_BUFFER_SIZE> txBuffer;

// ----------------------------------------------------------------------------
// called when the UART is ready to transmit the next byte
//
ISR(UART3_TRANSMIT_INTERRUPT)
{
	if (txBuffer.isEmpty())
	{
		// transmission finished, disable UDRE interrupt
		UART3_CONTROL &= ~(1 << UART3_UDRIE);
	}
	else {
		// get one byte from buffer and write it to UART (starts transmission)
		UART3_DATA = txBuffer.get();
		txBuffer.pop();
	}
}

// ----------------------------------------------------------------------------
void
xpcc::Uart3::put(char c)
{
	while (!txBuffer.push(c)) {
		// wait for a free slot in the buffer
	}
	
	atomic::Lock lock;
	
	// enable UDRE interrupt
	UART3_CONTROL |= (1 << UART3_UDRIE);
}

#endif	// ATMEGA_USART3
