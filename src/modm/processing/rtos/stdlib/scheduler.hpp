/*
 * Copyright (c) 2009, Georgi Grinshpun
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

#ifndef MODM_RTOS_BOOST_SCHEDULER_HPP
#define MODM_RTOS_BOOST_SCHEDULER_HPP

#ifndef MODM_RTOS_SCHEDULER_HPP
#	error "Don't include this file directly, use <modm/processing/rtos/scheduler.hpp>"
#endif

#include <thread>

namespace modm
{
	namespace rtos
	{
		class Scheduler
		{
		public:
			/**
			 * \brief	Starts the real time kernel
			 * 
			 * \warning	This function will never return.
			 */
			static void
			schedule();
		};
	}
}

#endif // MODM_RTOS_BOOST_SCHEDULER_HPP
