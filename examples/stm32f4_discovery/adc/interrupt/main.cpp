/*
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013, Kevin Laeufer
 * Copyright (c) 2013-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

typedef GpioInputA7 AdcIn;

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;
xpcc::log::Logger xpcc::log::info(loggerDevice);

static void
printAdc()
{
	Adc2::acknowledgeInterruptFlags(Adc2::InterruptFlag::All);

	const float maxVoltage = 3.3;
	float voltage = 0.0;
	int adcValue = 0;
	adcValue = Adc2::getValue();
	XPCC_LOG_INFO << "adcValue=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_INFO << " voltage=" << voltage << xpcc::endl;
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Uart2 for XPCC_LOG_INFO
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	// initialize Adc2
	Adc2::initialize<Board::systemClock>();
	AdcIn::connect(Adc2::Channel7);
	Adc2::setChannel(AdcIn::Adc2Channel);

	Adc2::enableInterruptVector(5);
	Adc2::enableInterrupt(Adc2::Interrupt::EndOfRegularConversion);
	AdcInterrupt2::attachInterruptHandler(printAdc);

	while (1)
	{
		Adc2::startConversion();

		xpcc::delayMilliseconds(500);
	}

	return 0;
}
