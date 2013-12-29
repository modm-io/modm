#include <xpcc/architecture/platform.hpp>
#include <xpcc/math.hpp>
#include "../stm32f4_discovery.hpp"

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);
	
	while (1)
	{
		xpcc::Vector<float, 2> v(1.0f, 2.0f);
		v.getLength();

		xpcc::Quaternion<float> q;
		q.getLength();
	}
}
