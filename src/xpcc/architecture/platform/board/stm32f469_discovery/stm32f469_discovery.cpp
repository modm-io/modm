/*
 * Copyright (c) 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
