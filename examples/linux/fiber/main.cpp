/*
 * Copyright (c) 2021, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/debug.hpp>
#include <modm/processing.hpp>

void hello()
{
	for(int ii=0; ii<10; ii++)
	{
		MODM_LOG_INFO << "Hello ";
		modm::fiber::yield();
	}
}

struct Test
{
	void world(const char *arg)
	{
		for(int ii=0; ii<10; ii++)
		{
			MODM_LOG_INFO << arg << modm::endl;
			modm::fiber::yield();
		}
	}
} test;

modm::Fiber<> fiber1(hello);
modm::fiber::Stack<> stack2;

int
main(void)
{
	const char *arg = "World";
	modm::fiber::Task fiber2(stack2, [=]() { test.world(arg); });

	MODM_LOG_INFO << "Start" << modm::endl;
	modm::fiber::Scheduler::run();
	MODM_LOG_INFO << "End" << modm::endl;

	return 0;
}
