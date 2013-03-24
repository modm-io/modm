
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

GPIO__INPUT(AdcIn0, B,  12);

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

	LedNorth::setOutput(xpcc::gpio::LOW);
	LedNorthEast::setOutput(xpcc::gpio::LOW);
	LedEast::setOutput(xpcc::gpio::LOW);
	LedSouthEast::setOutput(xpcc::gpio::LOW);
	LedSouth::setOutput(xpcc::gpio::LOW);
	LedSouthWest::setOutput(xpcc::gpio::LOW);
	LedWest::setOutput(xpcc::gpio::LOW);
	LedNorthWest::setOutput(xpcc::gpio::LOW);

	Adc4::initialize(Adc4::ClockMode::Asynchronous, Adc4::Prescaler::Div256,
					Adc4::CalibrationMode::SingleEndedInputsMode, true);

	Adc4::setChannel(Adc4::Channel::PinB12, Adc4::SampleTime::Cycles182);

	while (1)
	{
		Adc4::startConversion();
		while(!Adc4::isConversionFinished);
		Adc4::getValue();
	}

	return 0;
}
