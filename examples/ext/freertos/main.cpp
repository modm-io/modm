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
class BlinkThread: modm::rtos::Thread
{
	char c;
	uint8_t position;
	uint16_t delay;
	uint8_t i = 0;
	volatile float a = 10.f;
public:
	BlinkThread(char c, uint16_t delay, uint8_t position)
	: Thread(2, 1024), c{c}, position{position}, delay{delay} {}

	void run()
	{
		while (true)
		{
			sleep(delay * MILLISECONDS);

			Board::Leds::write((1ul << position) ^ Board::Leds::read());
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

BlinkThread p1('0', 260     , 0);
BlinkThread p2('a', 260 + 10, 1);
BlinkThread p3('A', 260 + 20, 2);
BlinkThread p4('!', 260 + 30, 3);


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::Leds::setOutput();

	MODM_LOG_INFO << "\n\nReboot: FreeRTOS blink example" << modm::endl;

	modm::rtos::Scheduler::schedule();
	return 0;
}
