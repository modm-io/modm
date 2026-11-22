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
#include <modm/processing/timer.hpp>
#include "../shared.hpp"

using namespace Board;
using namespace std::chrono_literals;

int
main()
{
	// Waits until the Cortex-M7 calls dualcore::start()
	Board::initialize();
	Shared &shared = dualcore::shared<Shared>();

	modm::PeriodicTimer timer{500ms};
	while (true)
	{
		if (shared.toM4.isNotEmpty() and shared.toM7.isNotFull())
		{
			shared.toM4.pop();
			uint32_t counter;
			{
				dualcore::Lock<1> lock;
				counter = ++shared.counter;
			}
			shared.toM7.push(counter);
		}
		if (timer.execute()) LedGreen::toggle();
		LedYellow::set(shared.button.load(std::memory_order_relaxed));
	}
	return 0;
}
