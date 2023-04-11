/*
 * Copyright (c) 2021, Jeff McBride
 * Copyright (c) 2022, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/io/iostream.hpp>

using namespace modm::platform;
using namespace modm::literals;

using Ad2 = GpioA19::Ad; // channel 8, pin AD2 on board
using Ad3 = GpioD30::Ad; // channel 0, pin AD3 on board

int main()
{
	Board::initialize();

	Afec0::initialize<Board::SystemClock>();
	Afec0::connect<Ad2::Ad, Ad3::Ad>();

	// Enable x4 oversampling. Output resolution of averaged samples is 13 bits.
	Afec0::setResolution(Afec0::Resolution::Average4_13Bit);

	while (true)
	{
		MODM_LOG_INFO << "ADC Readings: ";
		MODM_LOG_INFO.printf("%5d ", Afec0::readChannel(Afec0::getPinChannel<Ad2>()));
		MODM_LOG_INFO.printf("%5d ", Afec0::readChannel(Afec0::getPinChannel<Ad3>()));
		MODM_LOG_INFO << modm::endl;

		modm::delay(500ms);
	}
}
