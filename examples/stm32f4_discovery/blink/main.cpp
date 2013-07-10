
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedOrange, D, 13);		// User LED 3
GPIO__OUTPUT(LedGreen, D, 12);		// User LED 4
GPIO__OUTPUT(LedRed, D, 14);		// User LED 5
GPIO__OUTPUT(LedBlue, D, 15);		// User LED 6

GPIO__OUTPUT(VBusPresent, A, 9);		// green LED (LD7)
GPIO__OUTPUT(VBusOvercurrent, D, 5);	// red LED   (LD8)

GPIO__INPUT(Button, A, 0);
GPIO__OUTPUT(ClockOut, A, 8);
GPIO__OUTPUT(SystemClockOut, C, 9);

using namespace xpcc::stm32;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// New Static Clock Setup (S:: => Static)
	//typedef S::Pll<S::ExternalOscillator<MHz8>, MHz168, MHz48> clockSource;
	// typedef S::Pll<S::InternalClock<MHz16>, MHz168, MHz48> clockSource;
	typedef S::ExternalOscillator<MHz8> clockSource;
	// typedef S::InternalClock<MHz16> clockSource;
	StartupError err =
		S::SystemClock<clockSource>::enable();

	// Output clock source on PA8
	ClockOut::setAlternateFunction(Gpio::AF_0, Gpio::PUSH_PULL);
	S::MCO1::setDivision(S::MCO1::Division::By1);
	S::MCO1::connect(clockSource::Id);

	// Output SystemClock on PC9
	SystemClockOut::setAlternateFunction(Gpio::AF_0, Gpio::PUSH_PULL);
	S::MCO2::setDivision(S::MCO2::Division::By1);
	S::MCO2::connect(S::SystemClock<clockSource>::Id);

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
