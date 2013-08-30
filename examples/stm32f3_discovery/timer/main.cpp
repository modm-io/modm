
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
using namespace xpcc::stm32;

typedef GpioOutputE9 LedNorth;
typedef GpioOutputE8 LedNorthWest;


// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	StartupError err =
		SystemClock<Pll<ExternalOscillator<MHz8>, MHz72>>::enable();

	LedNorth::setOutput(xpcc::Gpio::HIGH);
	LedNorthWest::setOutput(xpcc::Gpio::LOW);


	Timer1::enable();
	//Timer1::setMode(Timer1::Mode::CenterAligned3);
	Timer1::setMode(Timer1::Mode::UpCounter);
	Timer1::setOverflow(0x8ff);
	//Timer1::setPeriod(4);
	Timer1::applyAndReset();
	Timer1::start();

	LedNorth::connect(Timer1::Channel1);
	LedNorthWest::connect(Timer1::Channel1N);
	Timer1::setCompareValue(1, 0x80);
	Timer1::configureOutputChannel(1,
			static_cast<uint32_t>(Timer1::OutputCompareMode::Pwm) | 0b1101);
	Timer1::setDeadTime(7);

	Timer1::enableOutput();

	while (1){}

	return 0;
}
