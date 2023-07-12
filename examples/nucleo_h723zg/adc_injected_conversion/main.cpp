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

int main()
{
	Board::initialize();

	Adc1::connect<A0::Inp15, A1::Inp10>();
	Adc1::initialize(Adc1::ClockMode::SynchronousPrescaler4,
					 Adc1::ClockSource::NoClock,
					 Adc1::Prescaler::Disabled,
					 Adc1::CalibrationMode::SingleEndedInputsMode);

	MODM_LOG_INFO << "ADC Injected Conversion Test\n";

	Adc1::setInjectedConversionSequenceLength(4);
	Adc1::setInjectedConversionChannel<A0>(0, Adc1::SampleTime::Cycles17);
	Adc1::setInjectedConversionChannel<A1>(1, Adc1::SampleTime::Cycles17);
	Adc1::setInjectedConversionChannel<A0>(2, Adc1::SampleTime::Cycles17);
	Adc1::setInjectedConversionChannel<A1>(3, Adc1::SampleTime::Cycles17);

	while (true) {
		// start regular conversion
		Adc1::setPinChannel<A1>(Adc1::SampleTime::Cycles17);
		Adc1::startConversion();

		Adc1::startInjectedConversionSequence();
		while (!Adc1::isInjectedConversionFinished());

		MODM_LOG_INFO << "ADC1 CH15 (injected): " << Adc1::getInjectedConversionValue(0) << '\n';
		MODM_LOG_INFO << "ADC1 CH15 (injected): " << Adc1::getInjectedConversionValue(2) << '\n';
		MODM_LOG_INFO << "ADC1 CH10 (injected): " << Adc1::getInjectedConversionValue(1) << '\n';
		MODM_LOG_INFO << "ADC1 CH10 (injected): " << Adc1::getInjectedConversionValue(3) << '\n';

		// wait for regular conversion to finish
		while (!Adc1::isConversionFinished());
		MODM_LOG_INFO << "ADC1 CH10 (regular): " << Adc1::getValue() << "\n\n";

		Leds::toggle();
		modm::delay_ms(500);
	}

	return 0;
}
