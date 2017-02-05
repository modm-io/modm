/*
 * Copyright (c) 2015-2017, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "stm32f769i_discovery.hpp"

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

	Board::LedJ13::setOutput();
	while(1) {
		Board::LedJ13::set();
		modm::delayMilliseconds(20);
		Board::LedJ13::reset();
		modm::delayMilliseconds(180);
	}
}
