
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
using namespace xpcc::lpc;

typedef GpioOutput0_7 Led;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// New Static Clock Setup ( => Static)
	typedef Pll<ExternalCrystal<MHz12>, MHz48> clockSource;
	// typedef ExternalOscillator<MHz12> clockSource;
	StartupError err =
		SystemClock<clockSource>::enable();

	ClockTree::connectToClockOut(ClockTree::ClockOutSource::SystemClock);
	ClockTree::setClockOutDivision(100);
	Gpio0_1::selectFunction(Gpio0_1::Function::ClockOut);	// TODO: replace with connect

	Led::setOutput();

	while (1)
	{
		Led::toggle();
		xpcc::delay_ms(100);
	}
}
