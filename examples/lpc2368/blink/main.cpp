/**
 * Test for LPC2368 processor on
 * Development board
 * "CP-JR ARM7 LPC2368"
 * from wwwww.etteam.com
 */

#include <xpcc/architecture.hpp>

GPIO__OUTPUT(LedA, 3, 25);
GPIO__OUTPUT(LedB, 3, 26);

// Test fixture to measure actual CPU frequency
__attribute__ ((section(".fastcode")))
__attribute__((noinline))
void pulse(void)
{
	while (1)
	{
		LedA::set();
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");
		LedA::reset();
		xpcc::delay_ms(100);
	}

}

int
main(void)
{
	// SystemInit();

	xpcc::lpc::Clock::initialize();

	// Set LED port pin to output
	LedA::setOutput();
	LedB::setOutput();

//	pulse();

	while (1)
	{
		LedA::reset();
		LedB::set();
		xpcc::delay_ms(100);
		LedA::set();
		LedB::reset();
		xpcc::delay_ms(900);
	}

	while(1);

	return 0;
}
