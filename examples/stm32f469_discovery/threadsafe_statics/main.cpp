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

void constructDummy();
class Dummy
{
public:
	Dummy()
	{
		MODM_LOG_INFO << "Dummy class constructed" << modm::endl;
		constructDummy(); // recursive initialization
	}
};

void
constructDummy()
{
	static Dummy dummy;
	MODM_LOG_INFO << "constructDummy() called" << modm::endl;
}

int
main()
{
	Board::initialize();
	uint32_t counter(0);

	while (true)
	{
		Board::LedGreen::toggle();
		modm::delay(Board::Button::read() ? 125ms : 500ms);

		MODM_LOG_INFO << "loop: " << counter++ << modm::endl;
		constructDummy();
	}

	return 0;
}
