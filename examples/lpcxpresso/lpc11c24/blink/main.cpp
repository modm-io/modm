
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(Led, 0, 7);

using namespace xpcc::lpc;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// New Static Clock Setup ( => Static)
	typedef Pll<ExternalOscillator<MHz12>, MHz48> clockSource;
	// typedef ExternalOscillator<MHz12> clockSource;
	StartupError err =
		SystemClock<clockSource>::enable();

	Led::setOutput();

	while (1)
	{
		Led::toggle();
		xpcc::delay_ms(500);
	}
}
