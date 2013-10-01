// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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

#ifndef XPCC_LPC11__SYSTICK_TIMER_HPP
#define XPCC_LPC11__SYSTICK_TIMER_HPP

#include <stdint.h>
#include "core.hpp"

namespace xpcc
{
	namespace lpc
	{
		/**
		 * @brief		SysTick Timer
		 * @ingroup		lpc
		 */
		class SysTickTimer
		{
		public:
			/**
			 * Enables the SysTick Timer to generate periodic events.
			 * 
			 * \param	reload
			 * 		Reload value to generate a 1000 Hz interrupt rate.
			 * 					
			 * 
			 * \warning	The SysTick Timer is used by default to increment
			 * 			xpcc::Clock, which is used by xpcc::Timeout and other
			 * 			similar processing classes.
			 * 			You must not increment the xpcc::Clock
			 * 			additionally somewhere else.
			 *
			 * In LPC11 the SysTick timer is normally fed with half of the
			 * system clock but it is changed to system clock here.
			 */
			static void
			enable(uint32_t reload = ((F_CPU / 1000) - 1));

			/**
			 * Disables SysTick Timer.
			 * 
			 * \warning	If the SysTick Timer is disabled xpcc::Clock is not
			 * 			incremented automatically. Workflow classes which
			 * 			rely on xpcc::Clock will not work if xpcc::Clock
			 * 			is not incremented.
			 */
			static void
			disable();

			/**
			 * Passed method will be called periodically on each event.
			 * Previously passed interrupt handler will be detached.
			 */
			static void
			attachInterrupt(InterruptHandler handler);

			/**
			 * Detaches previously attached interrupt handler.
			 */
			static void
			detachInterrupt();
		};
	}
}

#endif	//  XPCC_LPC11__SYSTICK_TIMER_HPP
