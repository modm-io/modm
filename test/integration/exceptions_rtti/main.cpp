/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <typeinfo>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedD13::setOutput(modm::Gpio::Low);
	uint32_t counter(0);

	MODM_LOG_INFO << "TypeId for LedD13: " << typeid(LedD13).name() << modm::endl;

	while (true)
	{
		LedBlue::toggle();
		modm::delay(500ms);

		try {
			throw counter++;
		}
		catch (uint32_t code) {
			MODM_LOG_ERROR << "Caught exception #" << code << modm::endl;
		}
	}

	return 0;
}
