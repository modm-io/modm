
#include <xpcc/architecture.hpp>

// ----------------------------------------------------------------------------
GPIO__OUTPUT(LedNorth,     E,  9); // LD3
GPIO__OUTPUT(LedNorthEast, E, 10); // LD5
GPIO__OUTPUT(LedEast,      E, 11); // LD7
GPIO__OUTPUT(LedSouthEast, E, 12); // LD9
GPIO__OUTPUT(LedSouth,     E, 13); // LD10
GPIO__OUTPUT(LedSouthWest, E, 14); // LD8
GPIO__OUTPUT(LedWest,      E, 15); // LD6
GPIO__OUTPUT(LedNorthWest, E,  8); // LD4

GPIO__INPUT(Button, A, 0);

using namespace xpcc::stm32;

static bool
initClock()
{
	// use external 8MHz clock from ST-LINK
	if (!Clock::enableHse(Clock::HseConfig::HSE_BYPASS)) {
		return false;
	}
	
	Clock::enablePll(Clock::PllSource::PLL_HSE, Clock::PllMul::MUL_9);
	return Clock::switchToPll();
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	initClock();

	LedNorth::setOutput(xpcc::Gpio::LOW);
	LedNorthEast::setOutput(xpcc::Gpio::HIGH);
	LedEast::setOutput(xpcc::Gpio::HIGH);
	LedSouthEast::setOutput(xpcc::Gpio::HIGH);
	LedSouth::setOutput(xpcc::Gpio::HIGH);
	LedSouthWest::setOutput(xpcc::Gpio::HIGH);
	LedWest::setOutput(xpcc::Gpio::HIGH);
	LedNorthWest::setOutput(xpcc::Gpio::HIGH);
	
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
