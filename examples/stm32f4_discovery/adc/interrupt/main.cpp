#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

#include "../../stm32f4_discovery.hpp"

typedef GpioInputA7 AdcIn;

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;
xpcc::log::Logger xpcc::log::info(loggerDevice);

static void
printAdc()
{
	Adc3::acknowledgeInterruptFlags(Adc3::InterruptFlag::All);

	const float maxVoltage = 3.3;
	float voltage = 0.0;
	int adcValue = 0;
	adcValue = Adc3::getValue();
	XPCC_LOG_INFO << "adcValue=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_INFO << " voltage=" << voltage << xpcc::endl;
}

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	// initialize Uart2 for XPCC_LOG_INFO
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<defaultSystemClock, 115200>(12);

	// initialize Adc3
	Adc3::initialize<defaultSystemClock>();
	AdcIn::connect(Adc2::Channel7);
	Adc2::setChannel(AdcIn::Adc2Channel);

	Adc3::enableInterruptVector(5);
	Adc3::enableInterrupt(Adc3::Interrupt::EndOfRegularConversion);
	AdcInterrupt3::attachInterruptHandler(printAdc);

	while (1)
	{
		Adc3::startConversion();

		xpcc::delayMilliseconds(500);
	}

	return 0;
}
