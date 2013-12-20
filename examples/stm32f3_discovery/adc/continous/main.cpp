
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

GPIO__INPUT(Adc1In, C,  0);
GPIO__INPUT(Adc2In, C,  2);
GPIO__INPUT(Adc3In, B, 13);
GPIO__INPUT(Adc4In, B, 12);

GPIO__INPUT(Button, A, 0);


/* Uncomment to use Interrupts */
// #define USE_INTERRUPTS

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
	adcValue = Adc1::getValue();
	XPCC_LOG_DEBUG << "Adc1: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_DEBUG << "; voltage=" << voltage << xpcc::endl;

/*
	adcValue = Adc2::getValue();
	XPCC_LOG_DEBUG << "Adc2: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_DEBUG << "; voltage=" << voltage << xpcc::endl;
	adcValue = Adc3::getValue();
	XPCC_LOG_DEBUG << "Adc3: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_DEBUG << "; voltage=" << voltage << xpcc::endl;
	adcValue = Adc4::getValue();
	XPCC_LOG_DEBUG << "Adc4: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_DEBUG << "; voltage=" << voltage << xpcc::endl;
*/
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

	LedNorth::setOutput(xpcc::Gpio::Low);
	LedNorthEast::setOutput(xpcc::Gpio::Low);
	LedEast::setOutput(xpcc::Gpio::Low);
	LedSouthEast::setOutput(xpcc::Gpio::Low);
	LedSouth::setOutput(xpcc::Gpio::Low);
	LedSouthWest::setOutput(xpcc::Gpio::Low);
	LedWest::setOutput(xpcc::Gpio::Low);
	LedNorthWest::setOutput(xpcc::Gpio::Low);

	Usart3::configurePins(Usart3::Mapping::REMAP_PB10_PB11);
	Usart3::setBaudrate(115200);

	Adc1::initialize(Adc1::ClockMode::Asynchronous, Adc1::Prescaler::Div128,
					Adc1::CalibrationMode::SingleEndedInputsMode, true);
	Adc1::setFreeRunningMode(true);
	Adc1In::setAnalogInput();
	Adc1::setChannel(Adc1::Channel::PinC0, Adc1::SampleTime::Cycles2);
	Adc1::startConversion();

	Adc2::initialize(Adc2::ClockMode::Asynchronous, Adc2::Prescaler::Div128,
					Adc2::CalibrationMode::SingleEndedInputsMode, true);
	Adc2::setFreeRunningMode(true);
	Adc2In::setAnalogInput();
	Adc2::setChannel(Adc2::Channel::PinC2, Adc2::SampleTime::Cycles2);
	Adc2::startConversion();

	Adc3::initialize(Adc3::ClockMode::Asynchronous, Adc3::Prescaler::Div128,
					Adc3::CalibrationMode::SingleEndedInputsMode, true);
	Adc3::setFreeRunningMode(true);
	Adc3In::setAnalogInput();
	Adc3::setChannel(Adc3::Channel::PinB13, Adc3::SampleTime::Cycles2);
	Adc3::startConversion();

	Adc4::initialize(Adc4::ClockMode::Asynchronous, Adc4::Prescaler::Div128,
					Adc4::CalibrationMode::SingleEndedInputsMode, true);
	Adc4::setFreeRunningMode(true);
	Adc4In::setAnalogInput();
	Adc4::setChannel(Adc4::Channel::PinB12, Adc4::SampleTime::Cycles2);
	Adc4::startConversion();

	while (1)
	{
		xpcc::delay_ms(200);
		printAdc();
	}

	return 0;
}
