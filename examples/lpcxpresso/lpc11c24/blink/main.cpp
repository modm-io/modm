
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
using namespace xpcc::lpc;

typedef GpioOutput0_7 Led;

// ----------------------------------------------------------------------------
int 
main()
{
	// New Static Clock Setup ( => Static)
	typedef Pll<ExternalCrystal<MHz12>, MHz48> clockSource;
	// typedef ExternalOscillator<MHz12> clockSource;
	SystemClock<clockSource>::enable();

	ClockControl::enableClockOutput(ClockControl::ClockOutputSource::SystemClock, 100);
	Gpio0_1::selectFunction(Gpio0_1::Function::ClockOutput);	// TODO: replace with connect

	Led::setOutput();

	while (1)
	{
		Led::toggle();
		xpcc::delayMilliseconds(100);
	}
}
