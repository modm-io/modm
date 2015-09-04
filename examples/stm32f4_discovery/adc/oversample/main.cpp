#include "../../stm32f4_discovery.hpp"
#include <xpcc/debug/logger.hpp>
#include <xpcc/processing.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

typedef GpioInputA7 AdcIn0;
typedef GpioInputA4 AdcIn1;
typedef GpioInputA2 AdcIn2;

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;
xpcc::log::Logger xpcc::log::info(loggerDevice);

// the three sensors are mapped: x = ch1, y = ch2, z = ch0
Adc2::Channel sensorMapping[3] = {
		Adc2::Channel::Channel7,
		Adc2::Channel::Channel4,
		Adc2::Channel::Channel2
};
// the results are up to 16 bit wide
uint32_t sensorData[3];
// 3 channels and averages of 100 oversamples
#include <xpcc/driver/adc/adc_sampler.hpp>
typedef xpcc::AdcSampler< AdcInterrupt2, 3, 100 > sensors;
xpcc::ShortTimeout timeout(100);

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	Board::initialize();

	// initialize Uart2 for XPCC_LOG_INFO
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	// initialize Adc2
	Adc2::initialize<Board::systemClock>();
	AdcIn0::connect(Adc2::Channel7);
	AdcIn1::connect(Adc2::Channel4);
	AdcIn2::connect(Adc2::Channel2);

	Adc2::enableInterruptVector(5);
	Adc2::enableInterrupt(Adc2::Interrupt::EndOfRegularConversion);

	sensors::initialize(sensorMapping, sensorData);
	sensors::startReadout();

	while (1)
	{
		if (timeout.isExpired() && sensors::isReadoutFinished())
		{
			uint32_t* data = sensors::getData();
			// send it via UART
			XPCC_LOG_INFO <<"x="  << data[0] << xpcc::endl;
			XPCC_LOG_INFO <<"y="  << data[1] << xpcc::endl;
			XPCC_LOG_INFO <<"z="  << data[2] << xpcc::endl;

			// start another readout
			sensors::startReadout();
			timeout.restart(200);
		}
	}

	return 0;
}
