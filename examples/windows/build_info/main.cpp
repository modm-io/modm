#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

#include <xpcc_build_info.hpp>

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

// SDLMain will generate WinMain function and call main bellow
int main(int argc, char** argv)
{
	// ensure the console has been allocated
	AllocConsole();

	// recover the (stolen) console from SDL
	freopen ("CON", "a", stdout);
	freopen ("CON", "r", stdin);
	freopen ("CON", "a", stderr);

	// Let's print some information that is provided in the xpcc_build_info.hpp
	XPCC_LOG_INFO << "Machine:  " << XPCC_BUILD_MACHINE << xpcc::endl;
	XPCC_LOG_INFO << "User:     " << XPCC_BUILD_USER << xpcc::endl;
	XPCC_LOG_INFO << "Os:       " << XPCC_BUILD_OS << xpcc::endl;
	XPCC_LOG_INFO << "Compiler: " << XPCC_BUILD_COMPILER << xpcc::endl;

	return 0;
}
