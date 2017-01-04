/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_RTOS__SCHEDULER_HPP
#define XPCC_RTOS__SCHEDULER_HPP

#include <modm/architecture/utils.hpp>

#ifdef XPCC__OS_HOSTED
#	include "boost/scheduler.hpp"
#elif defined(XPCC__CPU_CORTEX_M3) || defined(XPCC__CPU_CORTEX_M4)
#	include "freertos/scheduler.hpp"
#endif

#endif // XPCC_RTOS__SCHEDULER_HPP
