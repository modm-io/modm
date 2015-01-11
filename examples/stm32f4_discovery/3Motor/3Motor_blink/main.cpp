#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>
#include "../3Motor.hpp"

using namespace xpcc::stm32;

// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::Low);

	// initialize Uart1 for XPCC_LOG_
	DBG_UART3_TX::connect(Usart3::Tx);
	DBG_UART3_RX::connect(Usart3::Rx, Gpio::InputType::PullUp);
	Usart3::initialize<defaultSystemClock, 115200>(12);

	XPCC_LOG_DEBUG <<"3Motor blink & hardware test" << xpcc::endl;

	ADC12_IN4_BAT::connect(Adc1::Channel4);
	ADC12_IN5_12V::connect(Adc1::Channel5);
	ADC12_IN14_5V::connect(Adc1::Channel14);
	Adc1::initialize<defaultSystemClock>();
	Adc1::enableTemperatureRefVMeasurement();



	while (1)
	{
		LedRed::toggle();
		LedBlue::toggle();

		xpcc::delayMilliseconds(500);

		Adc1::setChannel( Adc1::Channel::Channel4, Adc1::SampleTime::Cycles144);
		Adc1::startConversion();
		while (!xpcc::stm32::Adc1::isConversionFinished());
		float vBat = Adc1::getValue() * VoltageSensors::voltPerTickBat;

		Adc1::setChannel( Adc1::Channel::Channel5, Adc1::SampleTime::Cycles144);
		Adc1::startConversion();
		while (!xpcc::stm32::Adc1::isConversionFinished());
		float v12V = Adc1::getValue() * VoltageSensors::voltPerTick12V;

		Adc1::setChannel( Adc1::Channel::Channel14, Adc1::SampleTime::Cycles144);
		Adc1::startConversion();
		while (!xpcc::stm32::Adc1::isConversionFinished());
		float v5V = Adc1::getValue() * VoltageSensors::voltPerTick5V;

		Adc1::setChannel( Adc1::Channel::Channel17 , Adc1::SampleTime::Cycles144);
		Adc1::startConversion();
		while (!xpcc::stm32::Adc1::isConversionFinished());
		float v3V3 = (4096.0 * 1.21) / Adc1::getValue();

		XPCC_LOG_DEBUG << "vBat=" << vBat << " v12V=" << v12V << " v5V=" << v5V << " v3V3=" << v3V3 << xpcc::endl;

	}

	return 0;
}
