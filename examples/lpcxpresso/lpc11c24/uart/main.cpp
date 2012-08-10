/**
 * \brief	Minimal example for LPC11C24 on LPCXpresso board.
 *
 *
 */

#include <xpcc/architecture.hpp>

GPIO__OUTPUT(Led, 0, 7);
GPIO__OUTPUT(WriteInd, 3, 1);

void
testWriteSingle(void);

void
testWriteBuffer(void);

extern xpcc::lpc::BufferedUart1 uart;

int
main(void)
{
	SystemInit();
	
	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	init_timer32(0, TIME_INTERVAL);
	
	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	enable_timer32(0);
	
	// Set LED port pin to output
	Led::setOutput();
	
	xpcc::lpc::BufferedUart1 uart(115200);

	xpcc::delay_ms(100); // glitch ?

	uart.write('X');
	uart.write('I');
	uart.write('X');

//	testWriteSingle();
	testWriteBuffer();

	while(1);

		// Single byte reading and writing, loopback.
//		uint8_t c;
//		if (uart.read(c))
//		{
//			uart.write(c);
//		}

		// Buffer reading and writing
//		uint8_t buf[16];
//		uint8_t rr = uart.read(buf, 16);
//		uart.write(buf, rr);
}

void
testWriteSingle(void)
{
	while (1)
	{
		Led::toggle();

		// Multiple single byte writes in blocking mode
		uart.write('\n');
		for (uint8_t ii = 'a'; ii < 't'; ++ii) {
			WriteInd::setOutput(true);
			uart.write(ii);
			WriteInd::setOutput(false);
		}

		xpcc::delay_ms(2);

		// Some pause between burst otherwise USB is overloaded.
		static uint8_t burst = 0;
		if (burst++ > 5)
		{
			burst = 0;
			xpcc::delay_ms(500);
		}
	} // while (1)
}

void
testWriteBuffer(void)
{
	uint8_t buf[40];
	for (uint8_t ii = 0; ii < 40; ++ii)
	{
		buf[ii] = ii + 'A';
	}

	while (1)
	{
		Led::toggle();

		WriteInd::setOutput(true);
		uart.write(buf, 40);
		WriteInd::setOutput(false);
		WriteInd::setOutput(true);
		uart.write('\n');
		WriteInd::setOutput(false);

		xpcc::delay_ms(2);

		// Some pause between burst otherwise USB is overloaded.
		static uint8_t burst = 0;
		if (burst++ > 5) {
			burst = 0;
			xpcc::delay_ms(5000);
		}
	} // while (1)
}
