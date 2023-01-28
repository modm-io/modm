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

typedef GpioInputB12 AdcIn0;

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;
modm::log::Logger modm::log::info(loggerDevice);

static void
printAdc()
{
	const float maxVoltage = 3.3;
	float voltage = 0.0;
	int adcValue = 0;
	adcValue = Adc4::getValue();
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

	// initialize Adc4
	Adc4::initialize(Adc4::ClockMode::Asynchronous, Adc4::Prescaler::Div256,
					Adc4::CalibrationMode::SingleEndedInputsMode, true);
	Adc4::connect<AdcIn0::In3>();
	Adc4::setPinChannel<AdcIn0>(Adc4::SampleTime::Cycles182);

	Adc4::enableInterruptVector(5);
	Adc4::enableInterrupt(Adc4::Interrupt::EndOfRegularConversion);

	AdcInterrupt4::attachInterruptHandler(printAdc);

	while (true)
	{
		Adc4::startConversion();
		modm::delay(500ms);
	}

	return 0;
}

