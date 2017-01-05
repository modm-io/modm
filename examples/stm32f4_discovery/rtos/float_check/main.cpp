/*
 * Copyright (c) 2014, Georgi Grinshpun
 * Copyright (c) 2014, Sascha Schade
 * Copyright (c) 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>
#include <modm/processing/rtos.hpp>

using namespace modm::stm32;
using namespace modm::cortex;

/**
 * Check FPU support in FreeRTOS for STM32F4.
 *
 * FreeRTOS included in modm had no out of the box support for FPU.
 * FPU support was added manually.
 *
 * This example uses four threads to check if task switching still works when
 * the FPU is used.
 *
 * How to test?
 * ------------
 * - Use STM32F4 Discovery Board
 * - Compile and program (scons program)
 * - Connect UART at PA2
 *
 * What to expect?
 * ---------------
 * - All our LEDs blinking at different rates, about 3 to 4 Hz
 * - A string at PA2 at 115200 baud
 *
 *  0aA!1bB"2cC#3dD$4eE%5fF&6gG'7hH(8iI9)jJ0*aA1!bB2"cC
 *
 * Each thread prints out a sequence
 *    0123456789
 *    abcdefghij
 *    ABCDEFGHIJ
 *    !"#$%&'()*
 * respectivly.
 *
 * What can fail?
 * --------------
 * When FreeRTOS is not compiled without STM32F4XX flag the support for the
 * extended stack frame is missing in FreeRTOS. Then the device crashes
 * in the first context switch. So the output '0' or nothing is expected.
 *
 */

// ----------------------------------------------------------------------------
template <typename Gpio, int SleepTime>
class P: modm::rtos::Thread {
private:
	char c;
	int i;

	// volatile so the the compiler does not optimise out this operation
	volatile float a = 1.0;
public:
	P(char c):
	Thread(2,1<<10),
	c(c)
	{

	}
	void run() {
		Gpio::setOutput();
		while (1) {
			sleep(SleepTime);

			Gpio::toggle();
			modm::stm32::Usart2::writeBlocking(i + c);

			i = (i+1)%10;

			// Do some float calculation.
			a = a*10.0;
		}
	}
};

P< GpioD12, 260      > p1('0');
P< GpioD13, 260 + 10 > p2('a');
P< GpioD14, 260 + 20 > p3('A');
P< GpioD15, 260 + 30 > p4('!');


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	GpioA2::connect(modm::stm32::Usart2::Tx);
	GpioA3::connect(modm::stm32::Usart2::Rx, Gpio::InputType::PullUp);
	modm::stm32::Usart2::initialize<Board::systemClock, modm::stm32::Usart2::B115200>(6);

    while (1)
    {
    	modm::rtos::Scheduler::schedule();
    }

    // should not get here!
    return 0;
}
