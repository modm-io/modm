/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013-2017, Niklas Hauser
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

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

typedef GpioInputA7 AdcIn;

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;
modm::log::Logger modm::log::info(loggerDevice);

static void
printAdc()
{
	Adc2::acknowledgeInterruptFlags(Adc2::InterruptFlag::All);

	const float maxVoltage = 3.3;
	float voltage = 0.0;
	int adcValue = 0;
	adcValue = Adc2::getValue();
	MODM_LOG_INFO << "adcValue=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	MODM_LOG_INFO << " voltage=" << voltage << modm::endl;
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Uart2 for MODM_LOG_INFO
	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::SystemClock, 115200_Bd>();

	// initialize Adc2
	Adc2::initialize<Board::SystemClock>();
	Adc2::connect<AdcIn::In7>();
	Adc2::setPinChannel<AdcIn>();

	Adc2::enableInterruptVector(5);
	Adc2::enableInterrupt(Adc2::Interrupt::EndOfRegularConversion);
	AdcInterrupt2::attachInterruptHandler(printAdc);

	while (true)
	{
		Adc2::startConversion();

		modm::delay(500ms);
	}

	return 0;
}
