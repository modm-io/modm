/*
 * Copyright (c) 2021, Raphael Lehmann
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "STM32L5 ADC basic example" << modm::endl;
	MODM_LOG_INFO << " running on Nucleo-L552ZE-Q" << modm::endl << modm::endl;

	MODM_LOG_INFO << "Configuring ADC ...";
	// max. ADC clock for STM32L552: 80 MHz
	// 110 MHz AHB clock / 2 = 55 MHz
	Adc1::initialize(
		Adc1::ClockMode::SynchronousPrescaler2,
		Adc1::ClockSource::SystemClock,
		Adc1::Prescaler::Disabled,
		Adc1::CalibrationMode::SingleEndedInputsMode,
		true);
	// GpioB1: A6
	Adc1::connect<GpioB1::In16>();
	Adc1::setPinChannel<GpioB1>(Adc1::SampleTime::Cycles13);

	while (true)
	{
		Adc1::startConversion();
		while(!Adc1::isConversionFinished())
			;
		const auto adcValue = Adc1::getValue();
		MODM_LOG_INFO << "adcValue=" << adcValue;
		const float voltage = adcValue * 3.3 / 0xfff;
		MODM_LOG_INFO << " voltage=";
		MODM_LOG_INFO.printf("%.3f\n", voltage);
		Board::Leds::toggle();
		modm::delay(500ms);
	}

	return 0;
}
