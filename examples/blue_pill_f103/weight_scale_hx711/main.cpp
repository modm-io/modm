/*
 * Copyright (c) 2020, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing.hpp>
#include <modm/driver/adc/hx711.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
using Usart1 = BufferedUart<UsartHal1, UartTxBuffer<256>>;
modm::IODeviceWrapper< Usart1, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);


struct hx711_config : public modm::hx711::Config
{
	using Sck  = GpioOutputA9;
	using Data = GpioInputA10;
	// static const modm::hx711::InputChannelAndGain mode = modm::hx711::InputChannelAndGain::ChA_64;
};
using Hx711 = modm::Hx711< hx711_config >;
Hx711 hx711;

modm::Fiber fiber_hx711([]
{
	while (true)
	{
		const int32_t result = hx711.singleConversion();
		MODM_LOG_DEBUG.printf("%" PRIi32 "\n", result);
	}
});

modm::Fiber fiber_blink([]
{
	while (true)
	{
		modm::this_fiber::sleep_for(1s);
		LedGreen::toggle();
	}
});

/*
 * Blinks the green user LED with 1 Hz while measuring.
 *
 * Make the following connections to HX711:
 *   Data PA10
 *   Clock PA9
 */
int
main()
{
	Board::initialize();

	// Initialize Uart1 for MODM_LOG_*
	Usart1::connect<GpioOutputB6::Tx, GpioInputB7::Rx>();
	Usart1::initialize<Board::SystemClock, 115200_Bd>();

	// Use the logging streams to print some messages.
	MODM_LOG_DEBUG << "HX711 demo" << modm::endl;

	hx711_config::Sck::setOutput();
	hx711_config::Data::setInput();

	modm::fiber::Scheduler::run();

	return 0;
}
