/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;

// Reads the internal voltage reference and temperature sensor channels,
// VREFINT is about 1.2 V, so about 1490 counts at VDDA = 3.3 V.
int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "ADC internal channels" << modm::endl;

	if (not Adc1::initialize()) MODM_LOG_ERROR << "ADC initialization failed!" << modm::endl;
	Adc1::enableInternalChannels();

	while (true)
	{
		Leds::toggle();
		modm::delay(500ms);
		const uint16_t vref = Adc1::readChannel(Adc1::Channel::InternalReference, Adc1::SampleTime::Cycles289);
		const uint16_t temp = Adc1::readChannel(Adc1::Channel::Temperature, Adc1::SampleTime::Cycles289);
		MODM_LOG_INFO.printf("VREFINT=%4u VSENSE=%4u VDDA=%4lumV\n", vref, temp, 1212ul * 4095 / vref);
	}
}
