/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include "../shared.hpp"

using namespace Board;
using namespace std::chrono_literals;

// Initialized by the startup code of the Cortex-M7 before main()
modm_section(".data_d3_sram") constinit Shared shared;

modm::Fiber fiber_ping([]
{
	for (uint32_t ping = 0;; ping++)
	{
		modm::this_fiber::poll([]{ return shared.toM4.isNotFull(); });
		{
			dualcore::Lock<1> lock;
			shared.counter++;
		}
		shared.toM4.push(ping);
		modm::this_fiber::sleep_for(1s);
	}
});

modm::Fiber fiber_pong([]
{
	while (true)
	{
		modm::this_fiber::poll([]{ return shared.toM7.isNotEmpty(); });
		MODM_LOG_INFO << "Cortex-M4 counted to " << shared.toM7.get() << modm::endl;
		shared.toM7.pop();
	}
});

modm::Fiber fiber_button([]
{
	while (true)
	{
		shared.button.store(Button::read(), std::memory_order_relaxed);
		LedRed::toggle();
		modm::this_fiber::sleep_for(100ms);
	}
});

int
main()
{
	Board::initialize();
	dualcore::start(shared);
	MODM_LOG_INFO << "Cortex-M7 started the Cortex-M4" << modm::endl;

	modm::fiber::Scheduler::run();
	return 0;
}
