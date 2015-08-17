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
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__DELAY_HPP
#define	XPCC__DELAY_HPP

#include <stdint.h>

#ifdef __DOXYGEN__

namespace xpcc
{
	/**
	 * \brief	Delay ns nanoseconds
	 * \ingroup	architecture
	 */
	void
	delayNanoseconds(uint16_t ns);

	/**
	 * \brief	Delay us microseconds
	 * \ingroup	architecture
	 */
	void
	delayMicroseconds(uint16_t us);

	/**
	 * \brief	Delay ms milliseconds
	 * \ingroup	architecture
	 */
	void
	delayMilliseconds(uint16_t ms);
}

#else // !__DOXYGEN__

#include <xpcc/architecture/detect.hpp>
#include <xpcc/architecture/utils.hpp>

#if defined(XPCC__CPU_AVR)

	#include <util/delay.h>

	namespace xpcc
	{
		void ALWAYS_INLINE
		delayNanoseconds(uint16_t /*ns*/)
		{
			_delay_us(1);
		}

		ALWAYS_INLINE void
		delayMicroseconds(uint16_t us)
		{
			while(us--) _delay_us(1);
		}

		ALWAYS_INLINE void
		delayMilliseconds(uint16_t ms)
		{
			while(ms--) _delay_ms(1);
		}
	}

#elif defined(XPCC__OS_UNIX) || defined(XPCC__OS_OSX)

	#include <unistd.h>

	namespace xpcc
	{
		void ALWAYS_INLINE
		delayNanoseconds(uint16_t /*ns*/)
		{
			usleep(1);
		}

		ALWAYS_INLINE void
		delayMicroseconds(uint16_t us)
		{
			usleep(us);
		}

		ALWAYS_INLINE void
		delayMilliseconds(uint16_t ms)
		{
			usleep(uint32_t(ms)*1000);
		}
	}

#elif defined(XPCC__OS_WIN32)

	namespace xpcc
	{
		/*inline void
		delayMicroseconds(int us)
		{
			int ms = us / 1000;
			if (ms <= 0) {
				ms = 1;
			}
			Sleep(ms);
		}

		inline void
		delay_ms(int ms)
		{
			Sleep(ms);
		}*/

		inline void
		delayNanoseconds(int)
		{
			// TODO
		}

		inline void
		delayMicroseconds(int)
		{
			// TODO
		}

		inline void
		delayMilliseconds(int)
		{
			// TODO
		}
	}

#elif defined(XPCC__CPU_ARM)

	extern "C" void _delay_ns(uint32_t ns);
	extern "C" void _delay_us(uint32_t us);
	extern "C" void _delay_ms(uint32_t ms);

	namespace xpcc
	{
		/// @warning    There is little to no timing guarantee with this method!
		ALWAYS_INLINE void
		delayNanoseconds(uint16_t ns)
		{
			::_delay_ns(ns);
		}

		ALWAYS_INLINE void
		delayMicroseconds(uint16_t us)
		{
			::_delay_us(us);
		}

		/// @warning    this method is _not_ guaranteed to work with inputs over 9000ms
		///             since "It's Over 9000"! (meaning 32bit arithmetics).
		ALWAYS_INLINE void
		delayMilliseconds(uint16_t ms)
		{
			::_delay_ms(ms);
		}
	}
#else
	#error "Unknown architecture, please add some specific delay functions!"
#endif

#endif	// !__DOXYGEN__
#endif	// XPCC__DELAY_HPP
