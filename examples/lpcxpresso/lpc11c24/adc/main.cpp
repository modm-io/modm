/*
 * Copyright (c) 2019, Sascha Schade
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
using namespace modm::literals;

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

// Create an IODeviceWrapper around the Uart Peripheral we want to use
modm::IODeviceWrapper< Uart1, modm::IOBuffer::BlockIfFull > loggerDevice;

// Set all four logger streams to use the UART
modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Board::LedRed::reset();

	// Enable UART 1
	Uart1::connect<GpioOutput1_7::Tx>();
	Uart1::initialize<Board::SystemClock, 9600_Bd>();

	// Enable Adc
	using Adc = modm::platform::AdcManualSingle;

	Adc::initialize();
	Adc::configurePins(0x01);

	while (true)
	{
		Board::LedRed::toggle();
		Adc::startConversion(Adc::ChannelMask::CHANNEL_0);
		while (not Adc::isConversionFinished() )
			{};
		uint16_t result = Adc::getValue();
		MODM_LOG_INFO << "Adc = " << result << modm::endl;
		modm::delayMilliseconds(100);
	}

	return 0;
}
