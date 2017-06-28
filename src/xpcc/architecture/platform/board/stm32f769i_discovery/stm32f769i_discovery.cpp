// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e. V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

//
// STM32F769I-DISCOVERY (MB1225B)
// Discovery kit for STM32F769 line
// http://www.st.com/content/st_com/en/products/evaluation-tools/product-evaluation-tools/mcu-eval-tools/stm32-mcu-eval-tools/stm32-mcu-discovery-kits/32f769idiscovery.html
//

#include "stm32f769i_discovery.hpp"

// Create an IODeviceWrapper around the Uart Peripheral we want to use
Board::LoggerDevice loggerDevice;

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

	Board::LedJ13::setOutput();
	while(1) {
		Board::LedJ13::set();
		xpcc::delayMilliseconds(20);
		Board::LedJ13::reset();
		xpcc::delayMilliseconds(180);
	}
}
