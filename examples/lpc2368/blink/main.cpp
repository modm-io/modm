/**
 * Test for LPC2368 processor on
 * Development board
 * "CP-JR ARM7 LPC2368"
 * from wwwww.etteam.com
 */

#include <xpcc/architecture.hpp>

GPIO__OUTPUT(LedA, 3, 25);
GPIO__OUTPUT(LedB, 3, 26);

// test if the fast GPIO mode for port 0 and port 1 works
GPIO__OUTPUT(PinA, 0, 15);
GPIO__OUTPUT(PinB, 0, 16);
GPIO__OUTPUT(PinC, 0, 17);
GPIO__OUTPUT(PinD, 0, 18);

// Push buttons
GPIO__INPUT(ButA, 4, 28);
GPIO__INPUT(ButB, 4, 29);

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
	PinA::setOutput();
	PinB::setOutput();
	PinC::setOutput();
	PinD::setOutput();

	 ButA::setInput();
	 ButB::setInput();

//	pulse();

	// Test Button
//	while(1)
//	{
//		if (ButA::read())
//			LedA::set();
//		else
//			LedA::reset();
//	}

	while (1)
	{
		LedA::reset();
		LedB::set();
		PinA::set();
		PinB::set();
		PinC::set();
		PinD::set();

		xpcc::delay_ms(100);

		LedA::set();
		LedB::reset();
		PinA::reset();
		PinB::reset();
		PinC::reset();
		PinD::reset();

		xpcc::delay_ms(900);
	}

	while(1);

	return 0;
}
