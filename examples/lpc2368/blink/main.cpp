/**
 * Test for LPC2368 processor on
 * Development board
 * "CP-JR ARM7 LPC2368"
 * from wwwww.etteam.com
 */

#include <xpcc/architecture.hpp>

GPIO__OUTPUT(LedA, 3, 25);
GPIO__OUTPUT(LedB, 3, 26);

int
main(void)
{
	// SystemInit();

	xpcc::lpc::Clock::initialize();

	// Set LED port pin to output
	LedA::setOutput();
	LedB::setOutput();

	while (1)
	{
		LedA::reset();
		LedB::set();
		xpcc::delay_ms(100);
		LedA::set();
		LedB::reset();
		xpcc::delay_ms(900);
	}
}
