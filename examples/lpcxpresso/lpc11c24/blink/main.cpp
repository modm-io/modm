
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
using namespace xpcc::lpc;

// ----------------------------------------------------------------------------
int 
main()
{
	Board::initialize();

	// Clock output at Gpio0_1 with 1/100th of the CPU frequency. 48 kHz should be observed.
	ClockControl::enableClockOutput(ClockControl::ClockOutputSource::SystemClock, 100);
	Gpio0_1::selectFunction(Gpio0_1::Function::ClockOutput);	// TODO: replace with connect

	while (true)
	{
		Board::LedRed::toggle();
		xpcc::delayMilliseconds(500);
	}
}
