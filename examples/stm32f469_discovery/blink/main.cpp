#include "../stm32f469_discovery.hpp"
#include <xpcc/debug/logger.hpp>

using namespace Board;

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart3, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	Board::initialize();
	LedD13::setOutput(xpcc::Gpio::Low);

	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages
	XPCC_LOG_DEBUG   << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << "error"   << xpcc::endl;

	uint32_t counter(0);

	while (1)
	{
		LedGreen::toggle();
		xpcc::delayMilliseconds(Button::read() ? 125 : 500);

		LedOrange::toggle();
		xpcc::delayMilliseconds(Button::read() ? 125 : 500);

		LedRed::toggle();
		xpcc::delayMilliseconds(Button::read() ? 125 : 500);

		LedBlue::toggle();
		xpcc::delayMilliseconds(Button::read() ? 125 : 500);
		LedD13::toggle();

		XPCC_LOG_INFO << "loop: " << counter++ << xpcc::endl;
	}

	return 0;
}
