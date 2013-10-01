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
	typedef Pll<ExternalOscillator<MHz8>, MHz72> clockSource;
	StartupError err =
		SystemClock<clockSource>::enable();

	LedNorth::setOutput(xpcc::Gpio::LOW);
	LedNorthEast::setOutput(xpcc::Gpio::HIGH);
	LedEast::setOutput(xpcc::Gpio::HIGH);
	LedSouthEast::setOutput(xpcc::Gpio::HIGH);
	LedSouth::setOutput(xpcc::Gpio::HIGH);
	LedSouthWest::setOutput(xpcc::Gpio::HIGH);
	LedWest::setOutput(xpcc::Gpio::HIGH);
	LedNorthWest::setOutput(xpcc::Gpio::HIGH);

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
