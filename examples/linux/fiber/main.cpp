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

#include <chrono>
#include <cmath>
#include <modm/debug/logger.hpp>
#include <modm/processing.hpp>

using namespace std::chrono_literals;

#define STACK_SIZE 1024

void
f1()
{
	MODM_LOG_INFO << "f1" << modm::endl;
	modm::yield();
	MODM_LOG_INFO << "f1 done" << modm::endl;
}

void
f2()
{
	MODM_LOG_INFO << "f2" << modm::endl;
	modm::yield();
	MODM_LOG_INFO << "f2 done" << modm::endl;
}

void
idle()
{
	MODM_LOG_INFO << "idle" << modm::endl;
	modm::yield();
	MODM_LOG_INFO << "all done" << modm::endl;
}

modm::fiber::Stack<STACK_SIZE> stack1, stack2, stack_idle;
modm::Fiber fiber1(stack1, &f1), fiber2(stack2, &f2), fiber_idle(stack_idle, &idle);

int
main(void)
{
	MODM_LOG_INFO << "Start" << modm::endl;
	modm::fiber::scheduler.start();
	// Will never get here.
	MODM_LOG_INFO << "End" << modm::endl;
	return 0;
}
