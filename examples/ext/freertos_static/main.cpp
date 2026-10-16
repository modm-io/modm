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
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

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
SemaphoreHandle_t semaphore = nullptr;
StaticSemaphore_t semaphoreBuffer;

template <int Led, int SleepTime>
class P
{
	char c;
	uint8_t i = 0;
	volatile float a = 10.f;
	TaskHandle_t task;
	StaticTask_t taskBuffer;
	StackType_t stack[1024/4];
public:
	P(char c): c(c)
	{
		xTaskCreateStatic(
			[](void* obj) { reinterpret_cast<P*>(obj)->run(); },
			"anon",
			(1024 / 4),
			this,
			2,
			stack,
			&taskBuffer);
	}

	void run()
	{
		while (true)
		{
			vTaskDelay(SleepTime * (configTICK_RATE_HZ / 1000.0));

			Board::Leds::write(Board::Leds::read() ^ (1ul << (Led % Board::Leds::width)));
			{
				xSemaphoreTake(semaphore, portMAX_DELAY);
				MODM_LOG_INFO << char(i + c);
				xSemaphoreGive(semaphore);
			}
			i = (i+1)%10;
			a *= 3.141f;
		}
	}
};

P< 0, 260      > p1('0');
P< 1, 260 + 10 > p2('a');
P< 2, 260 + 20 > p3('A');

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::Leds::setOutput();

	semaphore = xSemaphoreCreateMutexStatic(&semaphoreBuffer);
	vTaskStartScheduler();

	return 0;
}
