/*
 * Copyright (c) 2016-2017, Niklas Hauser
 * Copyright (c) 2017, Nick Sarten
 * Copyright (c) 2018, Carl Treudler
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

typedef GpioInputA0 AdcIn1;

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	Adc::connect<AdcIn1::In0>();
	Adc::initialize(Adc::ClockMode::PCLKDividedBy4, Adc::CalibrationMode::DoNotCalibrate);

	while (true)
	{
		LedD13::toggle();
		modm::delayMilliseconds(250);

		Adc::setPinChannel<AdcIn1>();
		Adc::startConversion();
		while(!Adc::isConversionFinished()) { }
		MODM_LOG_INFO << "ADC A0: " << Adc::getValue() << modm::endl;

	}

	return 0;
}
