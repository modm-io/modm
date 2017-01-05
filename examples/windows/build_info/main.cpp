#include <modm/architecture/architecture.hpp>
#include <modm/debug/logger.hpp>

#include <modm_build_info.hpp>

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

// SDLMain will generate WinMain function and call main bellow
int main(int argc, char** argv)
{
	// ensure the console has been allocated
	AllocConsole();

	// recover the (stolen) console from SDL
	freopen ("CON", "a", stdout);
	freopen ("CON", "r", stdin);
	freopen ("CON", "a", stderr);

	// Let's print some information that is provided in the modm_build_info.hpp
	MODM_LOG_INFO << "Machine:  " << MODM_BUILD_MACHINE << modm::endl;
	MODM_LOG_INFO << "User:     " << MODM_BUILD_USER << modm::endl;
	MODM_LOG_INFO << "Os:       " << MODM_BUILD_OS << modm::endl;
	MODM_LOG_INFO << "Compiler: " << MODM_BUILD_COMPILER << modm::endl;

	return 0;
}
