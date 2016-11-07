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

typedef GpioInputC0  Adc1In;
typedef GpioInputC2  Adc2In;
typedef GpioInputB13 Adc3In;
typedef GpioInputB12 Adc4In;

xpcc::IODeviceWrapper< Usart2, xpcc::IOBuffer::BlockIfFull > loggerDevice;
xpcc::log::Logger xpcc::log::info(loggerDevice);

static void
printAdc()
{
	const float maxVoltage = 3.3;
	float voltage = 0.0;
	int adcValue = 0;
	adcValue = Adc1::getValue();
	XPCC_LOG_INFO << "Adc1: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_INFO << "; voltage=" << voltage << xpcc::endl;

/*
	adcValue = Adc2::getValue();
	XPCC_LOG_INFO << "Adc2: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_INFO << "; voltage=" << voltage << xpcc::endl;
	adcValue = Adc3::getValue();
	XPCC_LOG_INFO << "Adc3: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_INFO << "; voltage=" << voltage << xpcc::endl;
	adcValue = Adc4::getValue();
	XPCC_LOG_INFO << "Adc4: value=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	XPCC_LOG_INFO << "; voltage=" << voltage << xpcc::endl;
*/
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

	// initialize Adc
	Adc1::initialize(Adc1::ClockMode::Asynchronous, Adc1::Prescaler::Div128,
					Adc1::CalibrationMode::SingleEndedInputsMode, true);
	Adc1::setFreeRunningMode(true);
	Adc1In::connect(Adc1::Channel6);
	Adc1::setChannel(Adc1In::Adc1Channel, Adc1::SampleTime::Cycles2);
	Adc1::startConversion();

	Adc2::initialize(Adc2::ClockMode::Asynchronous, Adc2::Prescaler::Div128,
					Adc2::CalibrationMode::SingleEndedInputsMode, true);
	Adc2::setFreeRunningMode(true);
	Adc2In::connect(Adc2::Channel8);
	Adc2::setChannel(Adc2In::Adc2Channel, Adc2::SampleTime::Cycles2);
	Adc2::startConversion();

	Adc3::initialize(Adc3::ClockMode::Asynchronous, Adc3::Prescaler::Div128,
					Adc3::CalibrationMode::SingleEndedInputsMode, true);
	Adc3::setFreeRunningMode(true);
	Adc3In::connect(Adc3::Channel5);
	Adc3::setChannel(Adc3In::Adc3Channel, Adc3::SampleTime::Cycles2);
	Adc3::startConversion();

	Adc4::initialize(Adc4::ClockMode::Asynchronous, Adc4::Prescaler::Div128,
					Adc4::CalibrationMode::SingleEndedInputsMode, true);
	Adc4::setFreeRunningMode(true);
	Adc4In::connect(Adc4::Channel3);
	Adc4::setChannel(Adc4In::Adc4Channel, Adc4::SampleTime::Cycles2);
	Adc4::startConversion();

	while (1)
	{
		xpcc::delayMilliseconds(200);
		printAdc();
	}

	return 0;
}
