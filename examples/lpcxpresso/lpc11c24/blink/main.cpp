
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(Led, 0, 7);

using namespace xpcc::lpc;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// Setup Clock
	ClockTree::enableSource(ClockTree::ClockSource::ExternalOscillator);
	ClockTree::enablePll(ClockTree::PllSource::ExternalOscillator, 3, 1);
	ClockTree::switchSystemClockTo(ClockTree::SystemClockSource::Pll);

	Led::setOutput();

	while (1)
	{
		Led::toggle();
		xpcc::delay_ms(500);
	}
}
