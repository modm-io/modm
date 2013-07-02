
#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>


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


// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

using namespace xpcc::stm32;

xpcc::IODeviceWrapper<Usart3> loggerDevice;
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);


static void
printAdc()
{
	const float maxVoltage = 3.3;
	float voltage = 0.0;
	int adcValue = 0;
	adcValue = Adc4::getValue();
	XPCC_LOG_DEBUG << "adcValue=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_DEBUG << " voltage=" << voltage << xpcc::endl;
}

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

	Usart3::configurePins(Usart3::Mapping::REMAP_PB10_PB11);
	Usart3::setBaudrate(115200);

	Adc4::initialize(Adc4::ClockMode::Asynchronous, Adc4::Prescaler::Div256,
					Adc4::CalibrationMode::SingleEndedInputsMode, true);

	Adc4::enableInterruptVector(5);
	Adc4::enableInterrupt(Adc4::Interrupt::EndOfRegularConversion);

	AdcIn0::setAnalogInput();
	Adc4::setChannel(Adc4::Channel::PinB12, Adc4::SampleTime::Cycles182);

	while (1)
	{
		Adc4::startConversion();
		while(!Adc4::isConversionFinished);
		printAdc();
		xpcc::delay_ms(500);
	}

	return 0;
}


extern "C" void
ADC4_IRQHandler(void)
{
	if (Adc4::getInterruptFlags() & Adc4::InterruptFlag::EndOfRegularConversion) {
		Adc4::resetInterruptFlags(Adc4::InterruptFlag::EndOfRegularConversion);
		printAdc();
	}
}
