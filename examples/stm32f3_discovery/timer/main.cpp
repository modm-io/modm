
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

	LedNorth::setOutput(xpcc::gpio::HIGH);
	LedNorthEast::setOutput(xpcc::gpio::LOW);
	LedEast::setOutput(xpcc::gpio::LOW);
	LedSouthEast::setOutput(xpcc::gpio::LOW);
	LedSouth::setOutput(xpcc::gpio::LOW);
	LedSouthWest::setOutput(xpcc::gpio::LOW);
	LedWest::setOutput(xpcc::gpio::LOW);
	LedNorthWest::setOutput(xpcc::gpio::LOW);



	Timer1::enable();
	//Timer1::setMode(Timer1::CENTER_ALIGNED_3);
	Timer1::setMode(Timer1::UP_COUNTER);
	Timer1::setOverflow(0x8ff);
	//Timer1::setPeriod(4);
	Timer1::applyAndReset();
	Timer1::start();

	LedNorth::setAlternateFunction(AF_2, PUSH_PULL);		// TIM1_CH1
	LedNorthWest::setAlternateFunction(AF_2, PUSH_PULL);	// TIM1_CH1N
	Timer1::setCompareValue(1, 0x80);
	Timer1::configureOutputChannel(1, Timer1::OUTPUT_PWM | 0b1101);
	Timer1::setDeadTime(7);



	Timer1::enableOutput();




	while (1){}

	return 0;
}
