
#include <xpcc/debug/logger.hpp>

// Use the predefined logging devices for the PC. You must include this
// file only once! Preferably in the main file. Otherwise you will get
// errors from the linker complaining about double implementations.
#include <xpcc/debug/logger/imp/std.hpp>

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

int
main()
{
	// Use the logging streams to print some messages.
	// Change XPCC_LOG_LEVEL above to enable or disable these messages 
	XPCC_LOG_DEBUG   << XPCC_FILE_INFO << "debug"   << xpcc::endl;
	XPCC_LOG_INFO    << XPCC_FILE_INFO << "info"    << xpcc::endl;
	XPCC_LOG_WARNING << XPCC_FILE_INFO << "warning" << xpcc::endl;
	XPCC_LOG_ERROR   << XPCC_FILE_INFO << "error"   << xpcc::endl;
	
	return 0;
}
