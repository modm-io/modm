/*
 * Copyright (c) 2017, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	// Output MSI Clock at MCO
	// 48 MHz / 16 = 3 MHz expected at PA8
	ClockControl::enableClockOutput(
		ClockControl::ClockOutputSource::MultiSpeedInternalClock,
		ClockControl::ClockOutputSourcePrescaler::Div16);

	GpioConnector<Peripheral::Rcc, GpioA8::Mco>::connect();

	LedGreen::set();

	Adc1::initialize(Adc1::ClockMode::SynchronousPrescaler1,
					Adc1::ClockSource::SystemClock,
					Adc1::Prescaler::Disabled,
					Adc1::CalibrationMode::SingleEndedInputsMode, true);
	Adc1::connect<GpioInputA0::In5>();
	Adc1::setChannel(Adc1::Channel::Channel5, Adc1::SampleTime::Cycles182);

	int loop(0);
	while (true)
	{
		LedGreen::toggle();
		modm::delayMilliseconds(Button::read() ? 250 : 500);

		Adc1::startConversion();
		// wait for conversion to finish
		while(not Adc1::isConversionFinished())
			{};
		// print result
		int adcValue = Adc1::getValue();
		MODM_LOG_INFO.printf("loop=%d: adcValue=%d\n", loop++, adcValue);
	}

	return 0;
}
