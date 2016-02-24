#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

#include <xpcc_build_info.hpp>

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

int
main()
{
	// Let's print some information that is provided in the xpcc_build_info.hpp
	XPCC_LOG_INFO << "Machine:  " << XPCC_BUILD_MACHINE  << xpcc::endl;
	XPCC_LOG_INFO << "User:     " << XPCC_BUILD_USER     << xpcc::endl;
	XPCC_LOG_INFO << "Os:       " << XPCC_BUILD_OS       << xpcc::endl;
	XPCC_LOG_INFO << "Compiler: " << XPCC_BUILD_COMPILER << xpcc::endl;

	XPCC_LOG_INFO << "Compare xpcc's printf and xpcc with glibc's printf" << xpcc::endl;

	int32_t ii = -42;

	char format_string[] = ">>%5d<<";

	XPCC_LOG_INFO << "xpcc  ";
	XPCC_LOG_INFO.printf(format_string, ii);
	XPCC_LOG_INFO << xpcc::endl;

	char buf[32] = {' '};
	sprintf(buf, format_string, ii);
	XPCC_LOG_INFO.printf("glibc %s", buf);
	XPCC_LOG_INFO << xpcc::endl;

	return 0;
}
