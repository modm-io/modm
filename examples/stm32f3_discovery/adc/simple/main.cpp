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

#include <modm/architecture/platform.hpp>
#include <modm/debug/logger.hpp>

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

typedef GpioInputB12 AdcIn0;

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;
xpcc::log::Logger xpcc::log::info(loggerDevice);

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Uart2 for XPCC_LOG_INFO
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<Board::systemClock, 115200>(12);

	// initialize Adc4
	Adc4::initialize(Adc4::ClockMode::Asynchronous, Adc4::Prescaler::Div256,
					Adc4::CalibrationMode::SingleEndedInputsMode, true);
	AdcIn0::connect(Adc4::Channel3);
	Adc4::setChannel(AdcIn0::Adc4Channel, Adc4::SampleTime::Cycles182);

	while (1)
	{
		Adc4::startConversion();
		// wait for conversion to finish
		while(!Adc4::isConversionFinished);
		// print result
		int adcValue = Adc4::getValue();
		XPCC_LOG_INFO << "adcValue=" << adcValue;
		float voltage = adcValue * 3.3 / 0xfff;
		XPCC_LOG_INFO << " voltage=" << voltage << xpcc::endl;
		xpcc::delayMilliseconds(500);
	}

	return 0;
}
