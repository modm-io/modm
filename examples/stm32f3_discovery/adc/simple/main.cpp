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

// ----------------------------------------------------------------------------

typedef GpioInputB12 AdcIn0;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	// initialize Adc4
	Adc4::initialize(Adc4::ClockMode::Asynchronous, Adc4::Prescaler::Div256,
					Adc4::CalibrationMode::SingleEndedInputsMode, true);
	Adc4::connect<AdcIn0::In3>();
	Adc4::setPinChannel<AdcIn0>(Adc4::SampleTime::Cycles182);

	while (true)
	{
		Adc4::startConversion();
		// wait for conversion to finish
		while(!Adc4::isConversionFinished());
		// print result
		int adcValue = Adc4::getValue();
		MODM_LOG_INFO << "adcValue=" << adcValue;
		float voltage = adcValue * 3.3 / 0xfff;
		MODM_LOG_INFO << " voltage=" << voltage << modm::endl;
		modm::delay(500ms);
	}

	return 0;
}
