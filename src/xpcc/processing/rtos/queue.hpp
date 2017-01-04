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

#ifndef XPCC_RTOS_QUEUE_HPP
#define XPCC_RTOS_QUEUE_HPP

#include <modm/architecture/utils.hpp>

#ifdef XPCC_OS_HOSTED
#	include "boost/queue.hpp"
#elif defined(XPCC_CPU_CORTEX_M3) || defined(XPCC_CPU_CORTEX_M4)
#	include "freertos/queue.hpp"
#endif

#endif // XPCC_RTOS_QUEUE_HPP
