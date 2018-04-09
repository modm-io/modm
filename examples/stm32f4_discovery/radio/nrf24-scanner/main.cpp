/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, 2017, Sascha Schade
 * Copyright (c) 2014-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/driver/radio/nrf24/nrf24_phy.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing/timer.hpp>
#include <inttypes.h>

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

#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO


// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

typedef GpioOutputE11 Ce;
typedef GpioOutputE12 Csn;



typedef modm::Nrf24Phy<SpiMaster2, Csn, Ce> nrf24hal;


int
main()
{
	Board::initialize();

	Csn::setOutput(modm::Gpio::High);
	Ce::setOutput(modm::Gpio::Low);

	Board::LedOrange::setOutput(modm::Gpio::Low);

	// Enable SPI 2
	SpiMaster2::connect<GpioB15::Mosi, GpioB14::Miso, GpioB13::Sck>();
	SpiMaster2::initialize<Board::systemClock, MHz10>();

	// Enable UART 2
	Usart2::connect<GpioA2::Tx>();
	Usart2::initialize<Board::systemClock, 115200>();


	// Initialize nRF24-HAL
	nrf24hal::initialize();

//	MODM_LOG_INFO << "Hello from nRF24-HAL example" << modm::endl;


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

//	modm::ShortPeriodicTimer divide_timer((max_channel*rx_settle)/1000 * divider);
	modm::ShortPeriodicTimer divide_timer(500);
	modm::ShortPeriodicTimer refreshTerminal(500);

	bool divide_now = false;

	MODM_LOG_INFO.printf("\033[2J");
	MODM_LOG_INFO.printf("\033[1;10H");
	MODM_LOG_INFO.printf("2.4GHz scanner");

	while (1)
	{
		if(divide_timer.execute())
			divide_now = true;

		max = 0;
		for(i = 0; i < max_channel; i++)
		{
			nrf24hal::writeRegister(nrf24hal::NrfRegister::RF_CH, i + channel_start);

			Ce::set();
			modm::delayMicroseconds(rx_settle);
			Ce::reset();
			modm::delayMicroseconds(2);
			channel_info[i] += 5*nrf24hal::readRegister(nrf24hal::NrfRegister::RPD);

			if(channel_info[i] > max)
				max = channel_info[i];

			if(channel_info[i])
				Board::LedOrange::toggle();

			if(divide_now)
				channel_info[i] /= divider;

		}

		if(divide_now)
			divide_now = false;



		if(refreshTerminal.execute())
		{
			MODM_LOG_INFO.printf("\033[2J");
			MODM_LOG_INFO.printf("\033[1;10H");
			MODM_LOG_INFO.printf("2.4GHz scanner");
			MODM_LOG_INFO.printf("   max: %" PRId32, max);

			for(i = 0; i < max_channel; i++)
			{
				MODM_LOG_INFO.printf("\033[%d;0H", i+2); // down one line

				j=0;
				MODM_LOG_INFO.printf("%02d ", channel_start + i);

				uint32_t width = max_width * ( (1.0 * channel_info[i]) / max);
				for(j = 0; j < width; j++)
				{
					MODM_LOG_INFO.printf("+");
				}
			}

		}

	}

	return 0;
}
