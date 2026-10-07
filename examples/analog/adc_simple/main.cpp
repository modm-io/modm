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

typedef GpioInputA7 AdcIn;

int
main()
{
	Board::initialize();

	// initialize Adc2
	Adc2::connect<AdcIn::In7>();
	Adc2::initialize<Board::SystemClock>();
	Adc2::setPinChannel<AdcIn>();

	while (true)
	{
		Adc2::startConversion();
		// wait for conversion to finish
		while(!Adc2::isConversionFinished());
		// print result
		int adcValue = Adc2::getValue();
		MODM_LOG_INFO << "adcValue=" << adcValue;
		float voltage = adcValue * 3.3 / 0xfff;
		MODM_LOG_INFO << " voltage=" << voltage << modm::endl;
		modm::delay(500ms);
	}

	return 0;
}
