/*
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
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

// The analog input and the ADC clock differ between the devices
#if defined STM32G4
// GpioA0: A0
using AdcInput = GpioA0;
using AdcSignal = AdcInput::In1;
// max. ADC clock for STM32G474: 60 MHz
// 170 MHz AHB clock / 4 = 42.5 MHz
constexpr auto AdcClockMode = Adc1::ClockMode::SynchronousPrescaler4;
#elif defined STM32H7
// GpioA3: A0, 16-bit ADC1
using AdcInput = GpioA3;
using AdcSignal = AdcInput::Inp15;
constexpr auto AdcClockMode = Adc1::ClockMode::SynchronousPrescaler4;
#define ADC_CLOCK_SOURCE NoClock
#define ADC_SAMPLE_TIME Cycles17
// this ADC has 16 bits
#define ADC_MAX_VALUE 0xffff
#elif defined STM32L4
// GpioA0: A0
using AdcInput = GpioA0;
using AdcSignal = AdcInput::In5;
// max. ADC clock for STM32L476: 80 MHz
// 48 MHz AHB clock / 1 = 48 MHz
constexpr auto AdcClockMode = Adc1::ClockMode::SynchronousPrescaler1;
#else
// GpioB1: A6
using AdcInput = GpioB1;
using AdcSignal = AdcInput::In16;
// max. ADC clock for STM32L552: 80 MHz
// 110 MHz AHB clock / 2 = 55 MHz
constexpr auto AdcClockMode = Adc1::ClockMode::SynchronousPrescaler2;
#endif

#ifndef ADC_CLOCK_SOURCE
#define ADC_CLOCK_SOURCE SystemClock
#define ADC_SAMPLE_TIME Cycles13
#define ADC_MAX_VALUE 0xfff
#endif

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "ADC basic example" << modm::endl << modm::endl;

	MODM_LOG_INFO << "Configuring ADC ...";
	Adc1::initialize(
		AdcClockMode,
		Adc1::ClockSource::ADC_CLOCK_SOURCE,
		Adc1::Prescaler::Disabled,
		Adc1::CalibrationMode::SingleEndedInputsMode,
		true);
	Adc1::connect<AdcSignal>();
	Adc1::setPinChannel<AdcInput>(Adc1::SampleTime::ADC_SAMPLE_TIME);

	while (true)
	{
		Adc1::startConversion();
		while(!Adc1::isConversionFinished())
			;
		const auto adcValue = Adc1::getValue();
		MODM_LOG_INFO << "adcValue=" << adcValue;
		const float voltage = adcValue * 3.3 / ADC_MAX_VALUE;
		MODM_LOG_INFO << " voltage=";
		MODM_LOG_INFO.printf("%.3f\n", (double)voltage);
		Board::Leds::toggle();
		modm::delay(500ms);
	}

	return 0;
}
