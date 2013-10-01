
#include <xpcc/architecture/platform.hpp>

using namespace xpcc::stm32;

typedef GpioOutputD13 LedOrange;	// User LED 3
typedef GpioOutputD12 LedGreen;		// User LED 4
typedef GpioOutputD14 LedRed;		// User LED 5
typedef GpioOutputD15 LedBlue;		// User LED 6

typedef GpioOutputA9 VBusPresent;		// green LED (LD7)
typedef GpioOutputD5 VBusOvercurrent;	// red LED (LD8)

typedef GpioInputA0 Button;
typedef GpioOutputA8 ClockOut;
typedef GpioOutputC9 SystemClockOut;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// New Static Clock Setup
	//typedef Pll<ExternalOscillator<MHz8>, MHz168, MHz48> clockSource;
	// typedef Pll<InternalClock<MHz16>, MHz168, MHz48> clockSource;
	typedef ExternalOscillator<MHz8> clockSource;
	// typedef InternalClock<MHz16> clockSource;
	StartupError err =
		SystemClock<clockSource>::enable();

	// Output clock source on PA8
	ClockOut::configure(Gpio::OutputType::PushPull);
	ClockOut::connect(MCO1::Id);
	MCO1::setDivision(MCO1::Division::By1);
	MCO1::connect(clockSource::Id);

	// Output SystemClock on PC9
	SystemClockOut::configure(Gpio::OutputType::PushPull);
	SystemClockOut::connect(MCO2::Id);
	MCO2::setDivision(MCO2::Division::By1);
	MCO2::connect(SystemClock<clockSource>::Id);

	LedOrange::setOutput(xpcc::Gpio::HIGH);
	LedGreen::setOutput(xpcc::Gpio::LOW);
	LedRed::setOutput(xpcc::Gpio::HIGH);
	LedBlue::setOutput(xpcc::Gpio::HIGH);

	while (1)
	{
		LedBlue::toggle();
		LedGreen::toggle();
		xpcc::delay_ms(500);
	}

	return 0;
}
