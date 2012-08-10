/**
 * \brief	Minimal example for LPC11C24 on LPCXpresso board.
 *
 *
 */

#include <xpcc/architecture.hpp>

GPIO__OUTPUT(Led, 0, 7);

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
	
	xpcc::lpc::Uart1 uart(115200);

	while (1)
	{
		Led::toggle();

		// Single byte writing
		uart.write('H');
//		uart.write('e');
//		uart.write('l');
//		uart.write('l');
//		uart.write('o');
		uart.write('\n');

		// Buffer writing
//		uart.write((uint8_t *)"World\n", 6);

		// Single byte reading and writing
//		uint8_t c;
//		if (uart.read(c))
//		{
//			uart.write(c);
//		}

		// Buffer reading and writing
		uint8_t buf[16];
		uint8_t rr = uart.read(buf, 16);
		uart.write(buf, rr);

		xpcc::delay_ms(500);
	}
}
