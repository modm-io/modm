
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../../hal/atomic/queue.hpp"
#include "../../../hal/atomic/lock.hpp"

#include "uart_defines.h"
#include "uart_defaults.h"

#include "../uart1.hpp"

#ifdef ATMEGA_USART1

static xpcc::atomic::Queue<char, UART1_TX_BUFFER_SIZE> txBuffer;

// ----------------------------------------------------------------------------
// called when the UART is ready to transmit the next byte
//
ISR(UART1_TRANSMIT_INTERRUPT)
{
	if (txBuffer.isEmpty())
	{
		// transmission finished, disable UDRE interrupt
		UART1_CONTROL &= ~(1 << UART1_UDRIE);
	}
	else {
		// get one byte from buffer and write it to UART (starts transmission)
		UART1_DATA = txBuffer.get();
		txBuffer.pop();
	}
}

// ----------------------------------------------------------------------------
void
xpcc::Uart1::put(char c)
{
	while (!txBuffer.push(c)) {
		// wait for a free slot in the buffer
	}
	
	atomic::Lock lock;
	
	// enable UDRE interrupt
	UART1_CONTROL |= (1 << UART1_UDRIE);
}

#endif	// ATMEGA_USART1
