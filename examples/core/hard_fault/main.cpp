/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

/*
Make sure to compile and upload the *debug* version of the firmware:

	scons program profile=debug

Then you can start GDB in TUI mode by default:

	scons debug profile=debug

Alternatively if you installed gdbgui you can use:

	scons debug profile=debug ui=gdbgui


Useful GDB Commands:

- info break (i b)           list all breakpoints
- break (b)                  set breakpoint; e.g. "b main.cpp:120"
- continue (c)               continue to next breakpoint
- layout split               display C/C++ code as well as assembler instructions in window
- layout regs                display registers in window
- list (l)                   show
- next (n)                   next line; do not step into
- step (s)                   steps into function
- nexti (ni)                 go to next machine instruction
- stepi (si)                 step one machine instruction
- print (p)                  print variable (i.e. p a, for reg0 p $r0, for reg2 in hex p/x $r0)
- set (s)                    set variable (i.e. s a=10, s $reg2=16)
- info registers (i r)       show registers and content
- info registers XX (i r XX) replace XX by register name


When the HardFault handler has stored a coredump, you can also debug the crash
without the hardware by following the instructions printed on the logger.

*/

#include <modm/board.hpp>
using namespace Board;

int global_a = 1;
int global_b = 2;

/* Can you debug this function chaos */
/* Do you dare to???                 */

void foo2(int &x, int &y)
{
	x += 10;
	y -= 10;
}

void foo1(int x, int y)
{
	x += 100;
	y += 30;
	foo2(x, y);
}

void foo3(int *x, int *y)
{
	(*x)++;
	(*y) += 200;
}

/* infinite loop ... or maybe not */
int foo_foo(int z)
{
	if(z == 42) return 42;
	z += 3;
	return foo_foo(z);
}

void function_chaos()
{
	int x = 4;
	int y = 6;

	foo1(x, y);
	foo2(x, y);
	foo3(&x, &y);

	foo_foo(0);
	foo_foo(1);
}

/* Come fun iterative / recoursive functions for you to debug */
int fib_itr(int a)
{
	if(a == 0) return 0;
	if(a == 1) return 1;
	int f0 = 0, f1 = 1, res = 2;

	for (int i = 2; i <= a; i++) {	// way better runtime (linear!!)
		res = f0 + f1;
		f0 = f1;
		f1 = res;
	}

	return res;
}

int fib_rec(int a)
{
	if(a == 0) return 0;
	if(a == 1) return 1;
	return fib_rec(a-1) + fib_rec(a-2);	// this has exponential runtime...
}

// x to the power of y
int pow(int x, int y)
{
	if(y <= 1) return x;
	return x * pow(x, y-1);
}

void rec_itr()
{
	int i = 0;

	i = fib_itr(10);
	i = fib_rec(10);
	i = pow(2, 30);	// = (1<<30)
	(void)i;
}

__attribute__((noinline))
void function1(uint32_t bla)
{
	static_cast<void>(bla);

	if (Button::read()) {
		// execute undefined instructed
		asm volatile (".short 0xde00");
	}
}

__attribute__((noinline))
void function2(uint32_t bla, uint8_t blub)
{
	static_cast<void>(blub);
	function1(bla);
}

void modm_hardfault_entry()
{
	// Put hardware in safe mode here
	Board::Leds::set();
	// But do not wait forever
	modm::delay(100ms);
	// Do not depend on interrupts in this function (buffered UART etc!)
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::Leds::setOutput(modm::Gpio::High);

	uint32_t *const ptr = new uint32_t[2*1024];
	MODM_LOG_INFO << "Can I allocate 2kB? answer: " << ptr << modm::endl;

	if (FaultReporter::hasReport())
	{
		MODM_LOG_ERROR << "\n\nHardFault! Copy the data into a 'coredump.txt' file, ";
		MODM_LOG_ERROR << "then execute\n\n\tscons debug-coredump ";
#ifdef MODM_DEBUG_BUILD
		MODM_LOG_ERROR << "profile=debug ";
#endif
		MODM_LOG_ERROR << "firmware=" << modm::hex;
		for (const auto data : FaultReporter::buildId()) MODM_LOG_ERROR << data;
		MODM_LOG_ERROR << "\n\n";
		for (const auto data : FaultReporter())
			MODM_LOG_ERROR << modm::hex << data << modm::flush;
		MODM_LOG_ERROR << "\n\n\n" << modm::flush;
		FaultReporter::clearAndReboot();
	}

	MODM_LOG_INFO << "Hold Button to cause a Hardfault!" << modm::endl;

#ifdef CoreDebug_DHCSR_C_DEBUGEN_Msk
	if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
		MODM_LOG_INFO << "Debugger connected!" << modm::endl;
	}
#endif

	while (true)
	{
		Board::Leds::toggle();

		// some functions to step through with the debugger
		rec_itr();
		function_chaos();

		// crashes if the button is pressed
		function2(23, 43);

		modm::delay(250ms);
	}

	return 0;
}
