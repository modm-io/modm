// coding: utf-8
/* Copyright (c) 2015-2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

//
// STM32F469I-DISCO
// Discovery kit for STM32F469 line
// http://www.st.com/web/catalog/tools/FM116/CL1620/SC959/SS1532/LN1848/PF262395
//

#include "stm32f469_discovery.hpp"
#include <xpcc/utils/bit_constants.hpp>

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Board::stlink::Uart, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

extern "C" void
xpcc_hook_hardware_init(void);

extern void
board_initialize_sdram();

void
xpcc_hook_hardware_init(void)
{
	// Reset LCD
	Board::DisplayReset::setOutput(xpcc::Gpio::Low);
	xpcc::delayMilliseconds(20);
	Board::DisplayReset::set();
	xpcc::delayMilliseconds(10);

	// initialize system clock and external SDRAM before accessing external memories
	Board::systemClock::enable();
	board_initialize_sdram();
}

xpcc_extern_c void
xpcc_abandon(const char * module,
			 const char * location,
			 const char * failure,
			 uintptr_t context)
{
	XPCC_LOG_ERROR << "Assertion '" << module << "." << location << "." << failure << "'";
	if (context) { XPCC_LOG_ERROR << " @ " << (void *) context << " (" << (uint32_t) context << ")"; }
	XPCC_LOG_ERROR << " failed! Abandoning..." << xpcc::endl;

	Board::LedBlue::setOutput();
	while(1) {
		Board::LedBlue::set();
		xpcc::delayMilliseconds(20);
		Board::LedBlue::reset();
		xpcc::delayMilliseconds(180);
	}
}
