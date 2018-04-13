#include <xpcc/architecture/platform.hpp>

using namespace Board;

#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::INFO

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages
	XPCC_LOG_DEBUG   << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << "error"   << xpcc::endl;

	LedGreen::set();

	uint32_t counter = 0;

	while (true)
	{
		LedGreen::toggle();
		xpcc::delayMilliseconds(500);
		XPCC_LOG_INFO << "loop: " << counter++ << xpcc::endl;
	}

	return 0;
}
