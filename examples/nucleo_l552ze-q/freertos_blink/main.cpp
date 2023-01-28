/*
 * Copyright (c) 2020, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing/rtos.hpp>

using namespace Board;

class BlinkTask : modm::rtos::Thread
{
public:
	BlinkTask()
		: Thread(4, 2048, "blink")
	{}

	void
	run()
	{
		Leds::setOutput();
		while (true)
		{
			vTaskDelay(500 * (configTICK_RATE_HZ / 1000.0));
			Leds::toggle();
		}
	}
};

BlinkTask blinkTask;

int
main()
{
	Board::initialize();

	MODM_LOG_INFO << "\n\nReboot: FreeRTOS blink example" << modm::endl;

	modm::rtos::Scheduler::schedule();

	// we should never get here
	return 0;
}
