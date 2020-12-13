/*
 * Copyright (c) 2014, Georgi Grinshpun
 * Copyright (c) 2015-2017, 2019 Niklas Hauser
 * Copyright (c) 2020, Ayoub SOUSSI
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

using namespace modm::platform;

/**
 * This example can be used as a template for FreeRtos + display implementation.
 *
 * What to expect?
 * ---------------
 * - All 3  LEDs blinking at different rates, about 3 to 4 Hz
 * - SSD1306 LCD screen displaying a counter.
 */

// ----------------------------------------------------------------------------
template <typename Gpio, int SleepTime>
class LedThread: modm::rtos::Thread
{
	char c;
	uint8_t i = 0;
	volatile float a = 10.f;
public:
	LedThread(char c): Thread(2,1<<10), c(c) {}

	void run()
	{
		Gpio::setOutput();
		while (true)
		{
			sleep(SleepTime * MILLISECONDS);

			Gpio::toggle();
			{
				static modm::rtos::Mutex lm;
				modm::rtos::MutexGuard m(lm);
			}
			i = (i+1)%10;
			a *= 3.141f;
		}
	}
};

LedThread< Board::LedRed,   260      > p1('0');
LedThread< Board::LedGreen, 260 + 10 > p2('a');
LedThread< Board::LedBlue,  260 + 20 > p3('A');


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	modm::rtos::Scheduler::schedule();
	return 0;
}
