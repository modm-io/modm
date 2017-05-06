/* Copyright (c) 2017, Sascha Schade (strongly-typed)
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------ */

#include "nucleo_f401re.hpp"

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Board::stlink::Uart, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

xpcc_extern_c void
xpcc_abandon(const char * module,
			 const char * location,
			 const char * failure,
			 uintptr_t context)
{
	XPCC_LOG_ERROR << "Assertion '" << module << "." << location << "." << failure << "'";
	if (context) { XPCC_LOG_ERROR << " @ " << (void *) context << " (" << (uint32_t) context << ")"; }
	XPCC_LOG_ERROR << " failed! Abandoning..." << xpcc::endl;

	// Since LedD13 is also a GPIO pin, we don't force this pin to output,
	// in case something sensitive is connected to this pin.
	// The user must "enable" the use of this pin as an LED output, by
	// explicitly setting the pin to output in the application.
	// Board::LedD13::setOutput();
	while(1) {
		Board::LedD13::set();
		xpcc::delayMilliseconds(20);
		Board::LedD13::reset();
		xpcc::delayMilliseconds(180);
	}
}
