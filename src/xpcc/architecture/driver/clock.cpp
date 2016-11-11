// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/detect.hpp>

#include "clock.hpp"

#if (XPCC__CLOCK_TESTMODE == 1)

	xpcc::Clock::Type xpcc::Clock::time = 0;

	template< typename TimestampType >
	TimestampType
	xpcc::Clock::now()
	{
		return TimestampType(time);
	}

#elif ( defined(XPCC__OS_UNIX) || defined(XPCC__OS_OSX) )
#	include <sys/time.h>

	template< typename TimestampType >
	TimestampType
	xpcc::Clock::now()
	{
		struct timeval now;
		gettimeofday(&now, 0);

		return TimestampType( now.tv_sec*1000 + now.tv_usec/1000 );
	}

#elif defined(XPCC__OS_WIN32) || defined(XPCC__OS_WIN64)
#	include <windows.h>

	template< typename TimestampType >
	TimestampType
	xpcc::Clock::now()
	{
		SYSTEMTIME now;
		GetSystemTime(&now);

		return TimestampType( now.wMilliseconds + now.wSecond*1000 + now.wMinute*1000*60 );
	}

#elif defined(XPCC__CPU_AVR) || defined(XPCC__CPU_ARM) || defined(XPCC__CPU_AVR32)
#	include <xpcc/architecture/driver/atomic/lock.hpp>

	xpcc::Clock::Type xpcc::Clock::time = 0;

	template< typename TimestampType >
	TimestampType
	xpcc::Clock::now()
	{
		typename TimestampType::Type tempTime;
		{
#if defined(XPCC__CPU_AVR)
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
template xpcc::ShortTimestamp xpcc::Clock::now();
template xpcc::Timestamp xpcc::Clock::now();
