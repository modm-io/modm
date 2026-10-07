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
using AdcIn = GpioInputA7;

static void
initializeAdc()
{
	MyAdc::connect<AdcIn::In7>();
	MyAdc::initialize<Board::SystemClock>();
	MyAdc::setPinChannel<AdcIn>();
}
#endif

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	initializeAdc();

	while (true)
	{
		MyAdc::startConversion();
		// wait for conversion to finish
		while(!MyAdc::isConversionFinished());
		// print result
		int adcValue = MyAdc::getValue();
		MODM_LOG_INFO << "adcValue=" << adcValue;
		float voltage = adcValue * 3.3 / 0xfff;
		MODM_LOG_INFO << " voltage=" << voltage << modm::endl;
		modm::delay(500ms);
	}

	return 0;
}
