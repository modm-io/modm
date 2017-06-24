/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2013, 2015-2017, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/platform.hpp>
#include <modm/architecture/interface/interrupt.hpp>
#include <modm/processing/timer.hpp>

using namespace modm::platform;

// create a output device for the led
typedef GpioOutputB0 Led;

// timer interrupt routine
MODM_ISR(TIMER2_COMPA)
{
	modm::Clock::increment();
}

int
main()
{
	Led::setOutput();
	Led::reset();

	// timer initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;

	// enable interrupts
	enableInterrupts();

	modm::ShortTimeout timeout(200);
	while (1)
	{
		if (timeout.isExpired())
		{
			timeout.restart(200);
			Led::toggle();
		}
	}
}
