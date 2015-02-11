#include <xpcc/architecture/platform.hpp>
#include "../../stm32f4_discovery.hpp"
#include <xpcc/driver/radio/nrf24/nrf24_phy.hpp>
#include <xpcc/debug/logger.hpp>
#include <xpcc/processing.hpp>

/*
 * A simple 2.4GHz "spectrum analyzer". Please use a terminal
 * application for UART monitoring such as picocom or screen
 *
 * Connect as follows
 *
 * STM32  NRF24
 * ------------
 * PB13 - SCK
 * PB14 - MISO
 * PB15 - MOSI
 * PE12 - CSN
 *
 * STM32  USB2UART
 * ---------------
 * PA2  - TXD
 */

#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO


// Create an IODeviceWrapper around the Uart Peripheral we want to use
xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
xpcc::log::Logger xpcc::log::debug(loggerDevice);
xpcc::log::Logger xpcc::log::info(loggerDevice);
xpcc::log::Logger xpcc::log::warning(loggerDevice);
xpcc::log::Logger xpcc::log::error(loggerDevice);

typedef GpioOutputE11 Ce;
typedef GpioOutputE12 Csn;



typedef xpcc::Nrf24Phy<SpiMaster2, Csn, Ce> nrf24hal;


MAIN_FUNCTION
{
	defaultSystemClock::enable();
	xpcc::cortex::SysTickTimer::enable();

	Csn::setOutput(xpcc::Gpio::High);
	Ce::setOutput(xpcc::Gpio::Low);

	LedOrange::setOutput(xpcc::Gpio::Low);

	// Enable SPI 2
	GpioOutputB15::connect(SpiMaster2::Mosi);
	GpioInputB14::connect(SpiMaster2::Miso);
	GpioOutputB13::connect(SpiMaster2::Sck);
	SpiMaster2::initialize<defaultSystemClock, MHz10>();

	// Enable UART 2
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<defaultSystemClock, 115200>(12);


	// Initialize nRF24-HAL
	nrf24hal::initialize();

//	XPCC_LOG_INFO << "Hello from nRF24-HAL example" << xpcc::endl;


	/*
	 * puts("\033[2J");   // clear screen
	 * puts("\033[0;0H"); // set cursor to 0,0
	 * puts("\033[10B");  // move cursor down 10 lines
	 * puts("\033[5A");  // move cursor up 5 lines
	 */

	nrf24hal::setBits(nrf24hal::NrfRegister::CONFIG, nrf24hal::Config::PWR_UP);
	nrf24hal::setBits(nrf24hal::NrfRegister::CONFIG, nrf24hal::Config::PRIM_RX);

	nrf24hal::writeRegister(nrf24hal::NrfRegister::EN_AA, 0x00);
	nrf24hal::writeRegister(nrf24hal::NrfRegister::RF_SETUP, 0x0f);


	constexpr const uint8_t channel_start = 25;
	constexpr const uint8_t channel_end = 80;
	constexpr const uint8_t max_channel = channel_end - channel_start;
	constexpr const uint32_t rx_settle = 250;
	constexpr const uint32_t divider = 2;
	constexpr const uint8_t max_width = 80;

	uint8_t i;
	uint8_t j;
	uint32_t max;

	uint32_t channel_info[max_channel];
	for(i = 0; i < max_channel; i++)
	{
		channel_info[i] = 0;
	}

//	xpcc::ShortPeriodicTimer divide_timer((max_channel*rx_settle)/1000 * divider);
	xpcc::ShortPeriodicTimer divide_timer(500);
	xpcc::ShortPeriodicTimer refreshTerminal(500);

	bool divide_now = false;

	XPCC_LOG_INFO.printf("\033[2J");
	XPCC_LOG_INFO.printf("\033[1;10H");
	XPCC_LOG_INFO.printf("2.4GHz scanner");

	while (1)
	{
		if(divide_timer.isExpired())
			divide_now = true;

		max = 0;
		for(i = 0; i < max_channel; i++)
		{
			nrf24hal::writeRegister(nrf24hal::NrfRegister::RF_CH, i + channel_start);

			Ce::set();
			xpcc::delayMicroseconds(rx_settle);
			Ce::reset();
			xpcc::delayMicroseconds(2);
			channel_info[i] += 5*nrf24hal::readRegister(nrf24hal::NrfRegister::RPD);

			if(channel_info[i] > max)
				max = channel_info[i];

			if(channel_info[i])
				LedOrange::toggle();

			if(divide_now)
				channel_info[i] /= divider;

		}

		if(divide_now)
			divide_now = false;



		if(refreshTerminal.isExpired())
		{
			XPCC_LOG_INFO.printf("\033[2J");
			XPCC_LOG_INFO.printf("\033[1;10H");
			XPCC_LOG_INFO.printf("2.4GHz scanner");
			XPCC_LOG_INFO.printf("   max: %d", max);

			for(i = 0; i < max_channel; i++)
			{
				XPCC_LOG_INFO.printf("\033[%d;0H", i+2); // down one line

				j=0;
				XPCC_LOG_INFO.printf("%02d ", channel_start + i);

				uint32_t width = max_width * ( (1.0 * channel_info[i]) / max);
				for(j = 0; j < width; j++)
				{
					XPCC_LOG_INFO.printf("+");
				}
			}

		}

	}

	return 0;
}
