
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedOrange, D, 13);		// User LED 3
GPIO__OUTPUT(LedGreen, D, 12);		// User LED 4
GPIO__OUTPUT(LedRed, D, 14);		// User LED 5
GPIO__OUTPUT(LedBlue, D, 15);		// User LED 6

GPIO__OUTPUT(VBusPresent, A, 9);		// green LED (LD7)
GPIO__OUTPUT(VBusOvercurrent, D, 5);	// red LED   (LD8)

GPIO__INPUT(Button, A, 0);

static void initClock(bool useInternal){
	typedef xpcc::stm32::Core::Clock MyCoreClock;
	if (useInternal){
		MyCoreClock::enablePll(MyCoreClock::PLL_HSI, 8, 168);
		MyCoreClock::switchToPll();
	}
	else{ // use external 8MHz crystal
		MyCoreClock::enableHSE(MyCoreClock::HSE_CRYSTAL);
		MyCoreClock::enablePll(MyCoreClock::PLL_HSE, 4, 168);
		MyCoreClock::switchToPll();
	}
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	LedOrange::setOutput(xpcc::gpio::HIGH);
	LedGreen::setOutput(xpcc::gpio::LOW);
	LedRed::setOutput(xpcc::gpio::HIGH);
	LedBlue::setOutput(xpcc::gpio::HIGH);
	
	initClock(false);

	while (1)
	{
		LedBlue::toggle();
		LedGreen::toggle();
		xpcc::delay_ms(500);
	}
}
