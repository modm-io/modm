/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/debug/logger.hpp>
#include <modm/board.hpp>

using namespace Board;

int main()
{
	Board::initialize();
	GpioPin<0>::setOutput();
	MODM_LOG_INFO << "Blink blink...";

	for (int i = 0; i < 10; ++i)
	{
		GpioPin<0>::toggle();
		modm::delay(500ms);
	}
	return 0;
}
