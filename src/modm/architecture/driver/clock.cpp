/*
 * Copyright (c) 2009-2011, 2016, Fabian Greif
 * Copyright (c) 2010, None
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2011-2012, 2014-2015, Niklas Hauser
 * Copyright (c) 2012, Dave Webb
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/detect.hpp>

#include "clock.hpp"

#if defined(MODM_CLOCK_TESTMODE) && (MODM_CLOCK_TESTMODE == 1)

	modm::Clock::Type modm::Clock::time = 0;

	template< typename TimestampType >
	TimestampType
	modm::Clock::now()
	{
		return TimestampType(time);
	}

#elif ( defined(MODM_OS_UNIX) || defined(MODM_OS_OSX) )
#	include <sys/time.h>

	template< typename TimestampType >
	TimestampType
	modm::Clock::now()
	{
		struct timeval now;
		gettimeofday(&now, 0);

		return TimestampType( now.tv_sec*1000 + now.tv_usec/1000 );
	}

#elif defined(MODM_OS_WIN32) || defined(MODM_OS_WIN64)
#	include <windows.h>

	template< typename TimestampType >
	TimestampType
	modm::Clock::now()
	{
		SYSTEMTIME now;
		GetSystemTime(&now);

		return TimestampType( now.wMilliseconds + now.wSecond*1000 + now.wMinute*1000*60 );
	}

#elif defined(MODM_CPU_AVR) || defined(MODM_CPU_ARM) || defined(MODM_CPU_AVR32)
#	include <modm/architecture/driver/atomic/lock.hpp>

	modm::Clock::Type modm::Clock::time = 0;

	template< typename TimestampType >
	TimestampType
	modm::Clock::now()
	{
		typename TimestampType::Type tempTime;
		{
#if defined(MODM_CPU_AVR)
			// only applicable for 8bit memory access
			// where reads are not atomic
			atomic::Lock lock;
#endif
			tempTime = typename TimestampType::Type(time);
		}
		return TimestampType(tempTime);
	}

#else
#	error	"Don't know how to create a Timestamp for this target!"
#endif

// explicit declaration of what member function templates we need to generate
template modm::ShortTimestamp modm::Clock::now();
template modm::Timestamp modm::Clock::now();
