/*
 * Copyright (c) 2024, Niklas Hauser
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

using namespace Board;
using namespace std::chrono_literals;


bool overflow{false};

modm::Fiber bad_fiber([]
{
	while(1)
	{
#ifdef __AVR__
		if (overflow) asm volatile ("push r1");
#else
		if (overflow) asm volatile ("push {r0-r7}");
#endif
		modm::this_fiber::yield();
	}
});

modm::Fiber blinky([]
{
	while(1)
	{
		Board::Leds::toggle();
		modm::this_fiber::sleep_for(0.5s);
		char c;
		modm::log::info.get(c);
		if (c == 'o') overflow = true;
	}
});

int
main()
{
	Board::initialize();
	Board::Leds::setOutput();
	MODM_LOG_INFO << "\nReboot!\nSend 'o' to overflow the stack!" << modm::endl;
	modm::delay(1s);

	modm::fiber::Scheduler::run();

	return 0;
}
