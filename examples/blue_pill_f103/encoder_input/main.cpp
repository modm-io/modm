/*
 * Copyright (c) 2021, Thomas Sommer
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
#include <modm/driver/encoder/encoder_input.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

int
main()
{
	Board::initialize();

	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::SystemClock, 115200_Bd>();

	// Each Timer can drive one Encoder
	// For Timer2 and Timer3 you have 2 Gpio options
	// When using one of PB3 or PB4 you also have to disable JTAG debugging during shared pins

	// Timer1:
	modm::EncoderInput<Timer1, GpioInputA8, GpioInputA9> encoder;

	// Timer2:
	// modm::EncoderInput<Timer2, GpioInputA0, GpioInputA1> encoder;
	// modm::EncoderInput<Timer2, GpioInputA15, GpioInputB3> encoder;
	// AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

	// Timer3:
	// modm::EncoderInput<Timer3, GpioInputA6, GpioInputA7> encoder;
	// modm::EncoderInput<Timer3, GpioInputB4, GpioInputB5> encoder;
	// Disable JTAG to make PB3, PB4 available as Gpio
	// AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

	// Timer4:
	// modm::EncoderInput<Timer4, GpioInputB6, GpioInputB7> encoder;

	encoder.initialize(true);

	modm::ShortPeriodicTimer heartbeat(500ms);

	while (true)
	{
		if(heartbeat.execute()) {
			Board::LedGreen::toggle();
			MODM_LOG_INFO << "Encoder Delta: " << encoder.getDelta() << modm::endl;
			MODM_LOG_INFO << "Encoder Absolut: " << encoder.getValue() << modm::endl;
		}
	}

	return 0;
}
