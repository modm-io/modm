#include <modm/board/board.hpp>

using namespace Board;

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	uint32_t counter(0);

	while (true)
	{
		LedD13::toggle();
		modm::delayMilliseconds(Button::read() ? 100 : 500);

		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
	}

	return 0;
}
