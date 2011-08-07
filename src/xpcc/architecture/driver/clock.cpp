// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/detect.hpp>

#include "clock.hpp"
#include "xpcc_config.hpp"

#if (XPCC__CLOCK_TESTMODE == 1)
	
	uint_fast16_t xpcc::Clock::time = 0;
	
	xpcc::Timestamp
	xpcc::Clock::now()
	{
		return Timestamp(time);
	}

#elif defined(XPCC__OS_UNIX)
#	include <sys/time.h>

	xpcc::Timestamp
	xpcc::Clock::now()
	{
		struct timeval now;
		gettimeofday(&now, 0);
		
		return Timestamp( now.tv_sec*1000 + now.tv_usec/1000 );
	}

#elif defined(XPCC__OS_WIN32) || defined(XPCC__OS_WIN64)
#	include <windows.h>

	xpcc::Timestamp
	xpcc::Clock::now()
	{
		SYSTEMTIME now;
		GetSystemTime(&now);
		
		return Timestamp( now.wMilliseconds + now.wSecond*1000 + now.wMinute*1000*60 );
	}
	
#elif defined(XPCC__CPU_AVR) || defined(XPCC__CPU_ARM)
#	include <xpcc/architecture/driver/atomic/lock.hpp>
	
	uint_fast16_t xpcc::Clock::time = 0;
	
	xpcc::Timestamp
	xpcc::Clock::now()
	{
		uint_fast16_t tempTime;
		{
			atomic::Lock lock;
			tempTime = time;
		}
		
		return Timestamp(tempTime);
	}
	
#else
#	error	"Don't know how to create a Timestamp for this target!"
#endif
