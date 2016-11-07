/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2011-2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "../mutex.hpp"

// ----------------------------------------------------------------------------
xpcc::rtos::Mutex::Mutex()
{
}

xpcc::rtos::Mutex::~Mutex()
{
}

// ----------------------------------------------------------------------------
bool
xpcc::rtos::Mutex::acquire(uint32_t timeout)
{
	return mutex.timed_lock(
			boost::posix_time::milliseconds(timeout));
}
