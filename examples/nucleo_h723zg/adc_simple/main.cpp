/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <cstdint>
#include <modm/board.hpp>

using namespace Board;

// ADC1 (16 bit) channel 15
using A0 = GpioA3;
// ADC3 (12 bit) channel 10
using A1 = GpioC0;

int main()
{
	Board::initialize();

	Adc1::connect<A0::Inp15>();
	Adc1::initialize(Adc1::ClockMode::SynchronousPrescaler4,
					 Adc1::ClockSource::NoClock,
					 Adc1::Prescaler::Disabled,
					 Adc1::CalibrationMode::SingleEndedInputsMode);

	Adc3::connect<A1::Inp10>();
	Adc3::initialize(Adc3::ClockMode::SynchronousPrescaler4,
					 Adc3::ClockSource::NoClock,
					 Adc3::Prescaler::Disabled,
					 Adc3::CalibrationMode::SingleEndedInputsMode);

	MODM_LOG_INFO << "ADC Test\n";

	while (true) {
		Adc1::setPinChannel<A0>(Adc1::SampleTime::Cycles17);
		Adc1::startConversion();
		while (!Adc1::isConversionFinished());
		MODM_LOG_INFO << "ADC1 CH15: " << Adc1::getValue() << '\n';

		Adc3::setPinChannel<A1>(Adc3::SampleTime::Cycles13);
		Adc3::startConversion();
		while (!Adc3::isConversionFinished());
		MODM_LOG_INFO << "ADC3 CH10: " << Adc3::getValue() << '\n';

		modm::delay_ms(500);
	}

	return 0;
}
