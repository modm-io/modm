#include <xpcc/architecture.hpp>

using namespace xpcc::stm32;

typedef GpioOutputE9  LedNorth;
typedef GpioOutputE10 LedNorthEast;
typedef GpioOutputE11 LedEast;
typedef GpioOutputE12 LedSouthEast;
typedef GpioOutputE13 LedSouth;
typedef GpioOutputE14 LedSouthWest;
typedef GpioOutputE15 LedWest;
typedef GpioOutputE8  LedNorthWest;
typedef GpioOutputA8  ClockOut;
typedef GpioInputA0   Button;

MAIN_FUNCTION
{
	typedef Pll<ExternalClock<MHz8>, MHz72> clockSource;
	StartupError err =
		SystemClock<clockSource>::enable();

	LedNorth::setOutput(xpcc::Gpio::Low);
	LedNorthEast::setOutput(xpcc::Gpio::High);
	LedEast::setOutput(xpcc::Gpio::High);
	LedSouthEast::setOutput(xpcc::Gpio::High);
	LedSouth::setOutput(xpcc::Gpio::High);
	LedSouthWest::setOutput(xpcc::Gpio::High);
	LedWest::setOutput(xpcc::Gpio::High);
	LedNorthWest::setOutput(xpcc::Gpio::High);

	// Output SystemClock on PA8
	ClockOut::setOutput();
	ClockOut::connect(MCO::Id);
	MCO::connect(clockSource::Id);

	while (1)
	{
		LedNorth::toggle();
		xpcc::delay_ms(100);
		LedNorthEast::toggle();
		xpcc::delay_ms(100);
		LedEast::toggle();
		xpcc::delay_ms(100);
		LedSouthEast::toggle();
		xpcc::delay_ms(100);
		LedSouth::toggle();
		xpcc::delay_ms(100);
		LedSouthWest::toggle();
		xpcc::delay_ms(100);
		LedWest::toggle();
		xpcc::delay_ms(100);
		LedNorthWest::toggle();
		xpcc::delay_ms(100);
	}

	return 0;
}
