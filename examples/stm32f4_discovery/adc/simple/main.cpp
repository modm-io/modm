#include "../../stm32f4_discovery.hpp"
#include <xpcc/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

typedef GpioInputA7 AdcIn;

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;
xpcc::log::Logger xpcc::log::info(loggerDevice);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Uart2 for XPCC_LOG_INFO
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	// initialize Adc4
	Adc2::initialize<Board::systemClock>();
	AdcIn::connect(Adc2::Channel7);

	Adc2::setChannel(AdcIn::Adc2Channel);

	while (1)
	{
		Adc2::startConversion();
		// wait for conversion to finish
		while(!Adc2::isConversionFinished());
		// print result
		int adcValue = Adc2::getValue();
		XPCC_LOG_INFO << "adcValue=" << adcValue;
		float voltage = adcValue * 3.3 / 0xfff;
		XPCC_LOG_INFO << " voltage=" << voltage << xpcc::endl;
		xpcc::delayMilliseconds(500);
	}

	return 0;
}
