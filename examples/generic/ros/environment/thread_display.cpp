/*
 * Copyright (c) 2017-2018, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <inttypes.h>
#include <modm/platform.hpp>
#include <modm/debug/logger.hpp>

#include "thread_display.hpp"

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DISABLED

// ----------------------------------------------------------------------------
DisplayThread::DisplayThread() :
	Fiber([this]{ this->update(); }),
	_dirty(true), _seq(0), _temp(0), _pres(0), _humi(0)
{
}

void
DisplayThread::update()
{
	// Wait for 100 msec unitl display powered up.
	modm::this_fiber::sleep_for(100ms);

	display.initializeBlocking();
	display.setFont(modm::font::Assertion);
	display.clear();
	display << "Hello World!";
	display.update();

	while(true)
	{
		modm::this_fiber::poll([&]{ return _dirty; });

		display.clear();

		display.printf("T=%2" PRId32 ".%02" PRId32 " C\n", (_temp/100),  (_temp%100));
		display.printf("P=%6" PRId32 ".%03" PRId32 " Pa\n", (_pres/1000), (_pres%1000));
		display.printf("H=%3" PRId32 ".%04" PRId32 " %%\n", (_humi/1000), (_pres%1000));
		display.printf("t=%" PRId32 " sec", _seq);
		display.update();

		_dirty = false;
	}
}
