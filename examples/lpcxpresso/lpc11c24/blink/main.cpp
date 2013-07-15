
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(Led, 0, 7);

using namespace xpcc::lpc;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// New Static Clock Setup (S:: => Static)
	typedef S::Pll<S::ExternalOscillator<MHz12>, MHz48> clockSource;
	// typedef S::ExternalOscillator<MHz12> clockSource;
	StartupError err =
		S::SystemClock<clockSource>::enable();

	Led::setOutput();

	while (1)
	{
		Led::toggle();
		xpcc::delay_ms(500);
	}
}
