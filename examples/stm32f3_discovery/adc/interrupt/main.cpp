#include <xpcc/architecture.hpp>
#include <xpcc/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

#include "../../stm32f3_discovery.hpp"

typedef GpioInputB12 AdcIn0;

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;
xpcc::log::Logger xpcc::log::info(loggerDevice);

static void
printAdc()
{
	const float maxVoltage = 3.3;
	float voltage = 0.0;
	int adcValue = 0;
	adcValue = Adc4::getValue();
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

	// initialize Adc4
	Adc4::initialize(Adc4::ClockMode::Asynchronous, Adc4::Prescaler::Div256,
					Adc4::CalibrationMode::SingleEndedInputsMode, true);

	Adc4::enableInterruptVector(5);
	Adc4::enableInterrupt(Adc4::Interrupt::EndOfRegularConversion);


	AdcIn0::connect(Adc4::Channel3);
	Adc4::setChannel(AdcIn0::Adc4Channel, Adc4::SampleTime::Cycles182);

	while (1)
	{
		Adc4::startConversion();
		while(!Adc4::isConversionFinished);
		printAdc();
		xpcc::delayMilliseconds(500);
	}

	return 0;
}


extern "C" void
ADC4_IRQHandler(void)
{
	if (Adc4::getInterruptFlags() & Adc4::InterruptFlag::EndOfRegularConversion) {
		Adc4::acknowledgeInterruptFlag(Adc4::InterruptFlag::EndOfRegularConversion);
		printAdc();
	}
}
