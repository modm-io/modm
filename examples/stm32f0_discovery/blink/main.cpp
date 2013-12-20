
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
using namespace xpcc::stm32;

typedef GpioInputA0 Button;
typedef GpioOutputA8 ClockOut;
typedef GpioOutputC8 LedBlue;
typedef GpioOutputC9 LedGreen;



// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	//typedef Pll<ExternalClock<MHz8>, MHz72> clockSource;
	//StartupError err =
	//	SystemClock<clockSource>::enable();

	LedBlue::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);

	// Output SystemClock on PA8
	// ClockOut::setOutput(Gpio::PUSH_PULL);
	// ClockOut::connect(MCO::Id);
	// MCO::connect(clockSource::Id);

	while (1)
	{
		LedBlue::toggle();
		LedGreen::toggle();
		xpcc::delay_ms(500);
	}

	return 0;
}
