#include "../stm32f4_discovery.hpp"
#include <xpcc/debug/logger.hpp>

/* This example showcases the hard fault handler LED blinking and UART logging.
 * You can trigger the hard fault by pressing the blue button, which will execute an undefined instruction.
 * The hard fault handler will report failure reason over PA2 @ 115.2kBaud and flash the blue LED.
 *
 * Connecting a debugger changes this behavior: the hardfault will now trigger a breakpoint.
 * This allows normal debugging of the failure reason.
 * You can try this yourself, open two shell in this folder, then:
 *  - in one terminal execute `scons openocd-debug`. This starts the openocd server.
 *  - then execute `scons debug`, which starts the GDB debugger in TUI mode.
 *  - type `monitor reset halt`, then `c` into GDB to restart the controller.
 * The red LED should now be on, signaling that the debugger has been recognized.
 * Pressing the blue button now, will make GDB jump to the breakpoint.
 *
 * You need to power cycle the board to reset the debugger recognition.
 */


// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedOrange::set();

	// initialize Uart2 for XPCC_LOG_
	GpioOutputA2::connect(Usart2::Tx);
	Usart2::initialize<Board::systemClock, 115200>(12);

	XPCC_LOG_INFO << "Press Button to cause a Hardfault!" << xpcc::endl;

	if (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) {
		// if this LED is on, the debugger is connected
		LedRed::set();
		XPCC_LOG_INFO << "Debugger connected!" << xpcc::endl;
	}

	while (1)
	{
		LedGreen::toggle();
		LedOrange::toggle();

		if (Button::read()) {
			// execute undefined instructed
			// the hard fault handler will blink the blue LED
			// or, if the debugger is connected, will trigger a breakpoint
			asm volatile (".short 0xde00");
		}

		xpcc::delayMilliseconds(500);

	}

	return 0;
}
