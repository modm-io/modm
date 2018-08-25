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

typedef GpioInputC0  Adc1In;
typedef GpioInputC2  Adc2In;
typedef GpioInputB13 Adc3In;
typedef GpioInputB12 Adc4In;

modm::IODeviceWrapper< Usart2, modm::IOBuffer::BlockIfFull > loggerDevice;
modm::log::Logger modm::log::info(loggerDevice);

static void
printAdc()
{
	const float maxVoltage = 3.3;
	float voltage = 0.0;
	int adcValue = 0;
	adcValue = Adc1::getValue();
	MODM_LOG_INFO << "Adc1: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	MODM_LOG_INFO << "; voltage=" << voltage << modm::endl;

/*
	adcValue = Adc2::getValue();
	MODM_LOG_INFO << "Adc2: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	MODM_LOG_INFO << "; voltage=" << voltage << modm::endl;
	adcValue = Adc3::getValue();
	MODM_LOG_INFO << "Adc3: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	MODM_LOG_INFO << "; voltage=" << voltage << modm::endl;
	adcValue = Adc4::getValue();
	MODM_LOG_INFO << "Adc4: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	MODM_LOG_INFO << "; voltage=" << voltage << modm::endl;
*/
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Uart2 for MODM_LOG_INFO
	Usart2::connect<GpioOutputA2::Tx>();
	Usart2::initialize<Board::systemClock, 115200>();

	// initialize Adc
	Adc1::initialize(Adc1::ClockMode::Asynchronous, Adc1::Prescaler::Div128,
					Adc1::CalibrationMode::SingleEndedInputsMode, true);
	Adc1::setFreeRunningMode(true);
	Adc1::connect<Adc1In::In6>();
	Adc1::setChannel(Adc1::Channel::Channel6, Adc1::SampleTime::Cycles2);
	Adc1::startConversion();

	Adc2::initialize(Adc2::ClockMode::Asynchronous, Adc2::Prescaler::Div128,
					Adc2::CalibrationMode::SingleEndedInputsMode, true);
	Adc2::setFreeRunningMode(true);
	Adc2::connect<Adc2In::In8>();
	Adc2::setChannel(Adc2::Channel::Channel8, Adc2::SampleTime::Cycles2);
	Adc2::startConversion();

	Adc3::initialize(Adc3::ClockMode::Asynchronous, Adc3::Prescaler::Div128,
					Adc3::CalibrationMode::SingleEndedInputsMode, true);
	Adc3::setFreeRunningMode(true);
	Adc3::connect<Adc3In::In5>();
	Adc3::setChannel(Adc3::Channel::Channel5, Adc3::SampleTime::Cycles2);
	Adc3::startConversion();

	Adc4::initialize(Adc4::ClockMode::Asynchronous, Adc4::Prescaler::Div128,
					Adc4::CalibrationMode::SingleEndedInputsMode, true);
	Adc4::setFreeRunningMode(true);
	Adc4::connect<Adc4In::In3>();
	Adc4::setChannel(Adc4::Channel::Channel3, Adc4::SampleTime::Cycles2);
	Adc4::startConversion();

	while (1)
	{
		modm::delayMilliseconds(200);
		printAdc();
	}

	return 0;
}
