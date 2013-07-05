
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedOrange, D, 13);		// User LED 3
GPIO__OUTPUT(LedGreen, D, 12);		// User LED 4
GPIO__OUTPUT(LedRed, D, 14);		// User LED 5
GPIO__OUTPUT(LedBlue, D, 15);		// User LED 6

GPIO__OUTPUT(VBusPresent, A, 9);		// green LED (LD7)
GPIO__OUTPUT(VBusOvercurrent, D, 5);	// red LED   (LD8)

GPIO__INPUT(Button, A, 0);

using namespace xpcc::stm32;

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	// New Static Clock Setup (S:: => Static)
	StartupError err =
		S::SystemClock<S::Pll<S::ExternalOscillator<MHz8>, MHz168, MHz48> >::enable();;

	LedOrange::setOutput(xpcc::Gpio::HIGH);
	LedGreen::setOutput(xpcc::Gpio::LOW);
	LedRed::setOutput(xpcc::Gpio::HIGH);
	LedBlue::setOutput(xpcc::Gpio::HIGH);
	
	while (1)
	{
		LedBlue::toggle();
		LedGreen::toggle();
		xpcc::delay_ms(1);
	}

	return 0;
}
