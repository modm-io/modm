#include "../stm32f469_discovery.hpp"

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
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
