/*
 * Copyright (c) 2020, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/driver/encoder/bitbang_encoder_output.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{

	initialize();

	LedBlue::set();
	LedGreen::set();

	// period=100000 // 100000us => 10Hz (visible)
	using TestEncoder = modm::BitBangEncoderOutput<LedBlue, LedGreen, int32_t, modm::PrecisePeriodicTimer, 100'000>;
	TestEncoder testEncoder{0};

	testEncoder.setPosition(1 << 31);
	while (1)
	{
		testEncoder.update();
		LedRed::toggle();
	}

	return 0;
}
