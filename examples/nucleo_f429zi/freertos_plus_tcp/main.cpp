/*
 * Copyright (c) 2014, Georgi Grinshpun
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2015-2017, 2019 Niklas Hauser
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
 * This example uses four threads to check if task switching works correctly.
 *
 * It also check if the FreeRTOS TCP stack can be compiled.
 * No TCP functionality in this example (yet).
 *
 * What to expect?
 * ---------------
 * - All our LEDs blinking at different rates, about 3 to 4 Hz
 * - A string at 115200 baud
 *
 *  0aA!1bB"2cC#3dD$4eE%5fF&6gG'7hH(8iI9)jJ0*aA1!bB2"cC
 *
 * Each thread prints out a sequence
 *    0123456789
 *    abcdefghij
 *    ABCDEFGHIJ
 *    !"#$%&'()*
 * respectivly.
 */

// ----------------------------------------------------------------------------
template <typename Gpio, int SleepTime>
class P: modm::rtos::Thread
{
	char c;
	uint8_t i = 0;
	volatile float a = 10.f;
public:
	P(char c): Thread(2,1<<10), c(c) {}

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
				MODM_LOG_INFO << char(i + c);
			}
			i = (i+1)%10;
			a *= 3.141f;
		}
	}
};

P< Board::LedRed,   260      > p1('0');
P< Board::LedGreen, 260 + 10 > p2('a');
P< Board::LedBlue,  260 + 20 > p3('A');


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	modm::rtos::Scheduler::schedule();
	return 0;
}
