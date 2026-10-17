/*
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2013, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

// ----------------------------------------------------------------------------

// The ADC, its analog input and its initialization differ between the devices
#if defined STM32F3
using MyAdc = Adc4;
using MyAdcInterrupt = AdcInterrupt4;
using AdcIn = GpioInputB12;

static void
initializeAdc()
{
	MyAdc::initialize(MyAdc::ClockMode::Asynchronous, MyAdc::Prescaler::Div256,
					  MyAdc::CalibrationMode::SingleEndedInputsMode, true);
	MyAdc::connect<AdcIn::In3>();
	MyAdc::setPinChannel<AdcIn>(MyAdc::SampleTime::Cycles182);
}
#else
using MyAdc = Adc2;
using MyAdcInterrupt = AdcInterrupt2;
using AdcIn = GpioInputA7;

static void
initializeAdc()
{
	MyAdc::connect<AdcIn::In7>();
	MyAdc::initialize<Board::SystemClock>();
	MyAdc::setPinChannel<AdcIn>();
}
#endif

static void
printAdc()
{
	const float maxVoltage = 3.3;
	float voltage = 0.0;
	int adcValue = 0;
#ifdef STM32F4
	MyAdc::acknowledgeInterruptFlags(MyAdc::InterruptFlag::All);
#endif
	adcValue = MyAdc::getValue();
	MODM_LOG_INFO << "adcValue=" << adcValue;
	voltage = adcValue * maxVoltage / 0xfff;
	MODM_LOG_INFO << " voltage=" << voltage << modm::endl;
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	initializeAdc();

	MyAdc::enableInterruptVector(5);
	MyAdc::enableInterrupt(MyAdc::Interrupt::EndOfRegularConversion);

	MyAdcInterrupt::attachInterruptHandler(printAdc);

	while (true)
	{
		MyAdc::startConversion();
		modm::delay(500ms);
	}

	return 0;
}

