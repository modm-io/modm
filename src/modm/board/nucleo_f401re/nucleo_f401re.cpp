/* Copyright (c) 2017, Sascha Schade (strongly-typed)
 * All Rights Reserved.
 *
 * The file is part of the modm library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------ */

#include "nucleo_f401re.hpp"

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Board::stlink::Uart, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

modm_extern_c void
modm_abandon(const char * module,
			 const char * location,
			 const char * failure,
			 uintptr_t context)
{
	MODM_LOG_ERROR << "Assertion '" << module << "." << location << "." << failure << "'";
	if (context) { MODM_LOG_ERROR << " @ " << (void *) context << " (" << (uint32_t) context << ")"; }
	MODM_LOG_ERROR << " failed! Abandoning..." << modm::endl;

	// Since LedD13 is also a GPIO pin, we don't force this pin to output,
	// in case something sensitive is connected to this pin.
	// The user must "enable" the use of this pin as an LED output, by
	// explicitly setting the pin to output in the application.
	// Board::LedD13::setOutput();
	while(1) {
		Board::LedD13::set();
		modm::delayMilliseconds(20);
		Board::LedD13::reset();
		modm::delayMilliseconds(180);
	}
}
