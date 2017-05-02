#include <xpcc/architecture/platform.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	// Output MSI Clock at MCO
	// 48 MHz / 16 = 3 MHz expected at PA8
	ClockControl::enableClockOutput(
		ClockControl::ClockOutputSource::MultiSpeedInternalClock,
		ClockControl::ClockOutputSourcePrescaler::Div16);

	xpcc::stm32::GpioA8::connect(xpcc::stm32::ClockControl::ClockOutput);

	LedGreen::set();

	while (true)
	{
		LedGreen::toggle();
		xpcc::delayMilliseconds(Button::read() ? 250 : 500);
	}

	return 0;
}
