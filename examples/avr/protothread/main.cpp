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
using namespace std::chrono_literals;

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

			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());

			LedGreen::reset();

			timeout.restart(600ms);
			PT_WAIT_UNTIL(timeout.isExpired());
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

			timeout.restart(200ms);
			PT_WAIT_UNTIL(timeout.isExpired());

			LedRed::reset();

			timeout.restart(300ms);
			PT_WAIT_UNTIL(timeout.isExpired());

			LedRed::set();

			timeout.restart(200ms);
			PT_WAIT_UNTIL(timeout.isExpired());

			LedRed::reset();

			timeout.restart(1s);
			PT_WAIT_UNTIL(timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
};

int
main()
{
	SystemClock::enable();
	enableInterrupts();

	BlinkingLightGreen greenLight;
	BlinkingLightRed redLight;
	while (true)
	{
		greenLight.run();
		redLight.run();
	}
}
