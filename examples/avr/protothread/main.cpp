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

#include <modm/platform.hpp>
#include <modm/architecture/interface/interrupt.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/processing/timer.hpp>

using namespace modm::platform;

typedef GpioOutputB0 LedGreen;
typedef GpioOutputB1 LedRed;

class BlinkingLightGreen : public modm::pt::Protothread
{
public:
	bool
	run()
	{
		PT_BEGIN();

		// set everything up
		LedGreen::setOutput();
		LedGreen::set();

		while (true)
		{
			LedGreen::set();

			this->timeout.restart(100);
			PT_WAIT_UNTIL(this->timeout.isExpired());

			LedGreen::reset();

			this->timeout.restart(600);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
};

class BlinkingLightRed : public modm::pt::Protothread
{
public:
	bool
	run()
	{
		PT_BEGIN();

		// set everything up
		LedRed::setOutput();
		LedRed::set();

		while (true)
		{
			LedRed::set();

			this->timeout.restart(200);
			PT_WAIT_UNTIL(this->timeout.isExpired());

			LedRed::reset();

			this->timeout.restart(300);
			PT_WAIT_UNTIL(this->timeout.isExpired());

			LedRed::set();

			this->timeout.restart(200);
			PT_WAIT_UNTIL(this->timeout.isExpired());

			LedRed::reset();

			this->timeout.restart(1000);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
};

// timer interrupt routine
MODM_ISR(TIMER2_COMPA)
{
	modm::Clock::increment();
}

int
main()
{
	// timeout initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;

	enableInterrupts();

	BlinkingLightGreen greenLight;
	BlinkingLightRed redLight;
	while (1)
	{
		greenLight.run();
		redLight.run();
	}
}
