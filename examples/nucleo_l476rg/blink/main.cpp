#include <modm/architecture/platform.hpp>

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

	modm::stm32::GpioA8::connect(modm::stm32::ClockControl::ClockOutput);

	LedGreen::set();

	while (true)
	{
		LedGreen::toggle();
		modm::delayMilliseconds(Button::read() ? 250 : 500);
	}

	return 0;
}
