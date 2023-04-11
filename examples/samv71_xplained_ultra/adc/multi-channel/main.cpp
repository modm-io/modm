/*
 * Copyright (c) 2023, Christopher Durand
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

/*
 * ADC example for multi-channel conversion sequences.
 *
 * Channels 0 and 8 are sampled in a sequence. The results are read from the
 * individual channel data registers after the sequence has completed.
 */

using Ad2 = GpioA19::Ad; // channel 8, pin AD2 on board
using Ad3 = GpioD30::Ad; // channel 0, pin AD3 on board

int main()
{
	Board::initialize();

	Afec0::initialize<Board::SystemClock>();
	Afec0::connect<Ad2::Ad, Ad3::Ad>();

	// Enable multi-channel sequence with Pins Ad3 (Ch 0) and Ad2 (Ch 8)
	Afec0::setPinChannels<Ad3, Ad2>();

	Afec0::startConversion();

	while (true)
	{
		if (Afec0::readInterruptStatus() & Afec0::Interrupt::Ch8EndOfConversion)
		{
			MODM_LOG_INFO << "ADC Readings: ";
			MODM_LOG_INFO.printf("AD2 %5d ", Afec0::getChannelValue<Ad2>());
			MODM_LOG_INFO.printf("AD3 %5d ", Afec0::getChannelValue<Ad3>());
			MODM_LOG_INFO << modm::endl;

			Afec0::startConversion();
		}
		modm::delay(500ms);
	}
}
