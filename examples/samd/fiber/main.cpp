/*
 * Copyright (c) 2020, Erik Henriksson
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/debug/logger.hpp>
#include <modm/processing.hpp>

using namespace Board;
using namespace std::chrono_literals;

#ifdef MODM_BOARD_HAS_LOGGER
#define STACK_SIZE 512
#else
// Without logging, we only need 4 byte (storing PC when performing a jump), plus space for
// another 8 registers (32 bytes), which are pushed to the stack by hardware when an interrupt
// happens (i.e. SysTick).
#define STACK_SIZE 40
#endif

void
f1();

void
f2();

modm::fiber::Stack<STACK_SIZE> stack1, stack2;
modm::Fiber fiber1(stack1, &f1), fiber2(stack2, &f2);

void
f1()
{
#ifdef MODM_BOARD_HAS_LOGGER
	MODM_LOG_INFO << "f1: entered" << modm::endl;
#endif
	while (1)
	{
		A0::set();
		modm::yield();
	}
}

void
f2()
{
#ifdef MODM_BOARD_HAS_LOGGER
	MODM_LOG_INFO << "f2: entered" << modm::endl;
#endif
	while (1)
	{
		A0::reset();
		modm::yield();
	}
}

// Frequency of A0 is 625.3kHz, resulting in ~45 CPU cycles per context switch (incl. GPIO
// overhead).
int
main(int argc, char* argv[])
{
	Board::initialize();
	A0::setOutput();
	modm::fiber::scheduler.start();
	// Will never get here.
	return 0;
}
