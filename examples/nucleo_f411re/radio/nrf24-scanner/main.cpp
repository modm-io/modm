/*
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2014, 2017, Sascha Schade
 * Copyright (c) 2014-2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../radio.hpp"

// A simple 2.4GHz "spectrum analyzer". Please use a terminal
// application for UART monitoring such as picocom or screen

int main()
{
	Board::initialize();
//	MODM_LOG_INFO << "Hello from nRF24 scanner example" << modm::endl;


	initializeSpi(1);
	// Initialize nRF24-HAL
	Nrf1Phy::initialize();


	/*
	 * puts("\033[2J");   // clear screen
	 * puts("\033[0;0H"); // set cursor to 0,0
	 * puts("\033[10B");  // move cursor down 10 lines
	 * puts("\033[5A");  // move cursor up 5 lines
	 */

	Nrf1Phy::setBits(Nrf1Phy::NrfRegister::CONFIG, Nrf1Phy::Config::PWR_UP);
	Nrf1Phy::setBits(Nrf1Phy::NrfRegister::CONFIG, Nrf1Phy::Config::PRIM_RX);

	Nrf1Phy::writeRegister(Nrf1Phy::NrfRegister::EN_AA, 0x00);
	Nrf1Phy::writeRegister(Nrf1Phy::NrfRegister::RF_SETUP, 0x0f);


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
	modm::ShortPeriodicTimer divide_timer(500ms);
	modm::ShortPeriodicTimer refreshTerminal(500ms);

	bool divide_now = false;

	MODM_LOG_INFO.printf("\033[2J");
	MODM_LOG_INFO.printf("\033[1;10H");
	MODM_LOG_INFO.printf("2.4GHz scanner");

	while (true)
	{
		if(divide_timer.execute())
			divide_now = true;

		max = 0;
		for(i = 0; i < max_channel; i++)
		{
			Nrf1Phy::writeRegister(Nrf1Phy::NrfRegister::RF_CH, i + channel_start);

			Nrf1Ce::set();
			modm::delay_us(rx_settle);
			Nrf1Ce::reset();
			modm::delay(2us);
			channel_info[i] += 5*Nrf1Phy::readRegister(Nrf1Phy::NrfRegister::RPD);

			if(channel_info[i] > max)
				max = channel_info[i];

			if(channel_info[i])
				Board::LedD13::toggle();

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
