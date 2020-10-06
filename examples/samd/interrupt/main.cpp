/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>

using namespace Board;
using namespace std::chrono_literals;

static volatile bool blink = true;

void
isr()
{
	blink = !blink;
	// Kids, please don't do serial logging in ISRs...
	MODM_LOG_DEBUG << "blink: " << (blink ? "true" : "false") << modm::endl;
}

int
main()
{
	Board::initialize();
	ExternalInterrupt::initialize();
	ExtInt<3>::initialize(&isr);
	ExtInt<3>::connect<D12>();
	while (1)
	{
		if (blink)
		{
			Led::toggle();
		} else
		{
			Led::set(0);
		}
		modm::delay(100ms);
	}
	return 0;
}
