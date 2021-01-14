/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include <modm/board.hpp>

using namespace Board;

int
main()
{
	Board::initialize();

	Adc::initialize<Board::SystemClock, Adc::ClockMode::Synchronous, 12_MHz>();
	const uint16_t Vref = Adc::readInternalVoltageReference();

	while (true)
	{
		int16_t Temp = Adc::readTemperature(Vref);
		LedOrange::set(Temp > 30);
		modm::delay(1s);
	}

	return 0;
}
