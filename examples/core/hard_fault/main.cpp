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

		// crashes if the button is pressed
		function2(23, 43);

		modm::delay(250ms);
	}

	return 0;
}
