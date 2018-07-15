/*
 * Copyright (c) 2018, Raphael Lehmann
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

#include <modm/platform/comp/comp_2.hpp>

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
	Board::LedNorth::setOutput();

	// initialize Uart2 for MODM_LOG_
	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::systemClock, 115200>();

	// Use the logging streams to print some messages.
	// Change MODM_LOG_LEVEL above to enable or disable these messages
	MODM_LOG_DEBUG   << "debug"   << modm::endl;
	MODM_LOG_INFO    << "info"    << modm::endl;
	MODM_LOG_WARNING << "warning" << modm::endl;
	MODM_LOG_ERROR   << "error"   << modm::endl;

	using Comparator = modm::platform::Comp2;

	Comparator::connect<GpioA7::Inp, GpioA2::Out>();

	Comparator::initialize(
			Comparator::InvertingInput::Vref1Div2,
			Comparator::NonInvertingInput::BitUnset, // GpioA7
			Comparator::Output::Tim1Or8BkIn2,
			Comparator::Hysteresis::NoHysteresis,
			Comparator::Mode::HighSpeed,
			Comparator::Polarity::NonInverted,
			false);

	while (1)
	{
		modm::delayMilliseconds(250);
		MODM_LOG_INFO << "Comparator: " << Comparator::getOutput() << modm::endl;
		Board::LedNorth::set(Comparator::getOutput());
	}

	return 0;
}
