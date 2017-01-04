/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef THREAD_BLINK
#define THREAD_BLINK

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

class BlinkThread: public xpcc::pt::Protothread
{
public:
	BlinkThread();

	bool
	update();

private:
	xpcc::ShortTimeout timeout;
	xpcc::ShortPeriodicTimer timer;
	uint32_t uptime;
};

#endif // THREAD_BLINK
