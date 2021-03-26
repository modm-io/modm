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
#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/driver/adc/hx711.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

// Create an IODeviceWrapper around the Uart Peripheral we want to use
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

class Hx711Thread : public modm::pt::Protothread
{
public:
	bool
	run()
	{
		PT_BEGIN();

		while (true)
		{
			result = PT_CALL(hx711.singleConversion());
			MODM_LOG_DEBUG.printf("%" PRIi32 "\n", result);
		}

		PT_END();
	}

protected:
	Hx711 hx711;
	int32_t result;
};

Hx711Thread hx711_thread;


class BlinkThread : public modm::pt::Protothread
{
public:
	bool
	run()
	{
		PT_BEGIN();

		while (true) {
			PT_WAIT_UNTIL(timer.execute());
			LedGreen::toggle();
		}

		PT_END();
	}

protected:
	modm::ShortPeriodicTimer timer{1s};
};

BlinkThread blink_thread;


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
	MODM_LOG_DEBUG   << "HX711 demo"   << modm::endl;

	hx711_config::Sck::setOutput();
	hx711_config::Data::setInput();

	LedGreen::set();

	while (true)
	{
		blink_thread.run();
		hx711_thread.run();
	}

	return 0;
}
