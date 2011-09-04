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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_FREERTOS__SCHEDULER_HPP
#define XPCC_FREERTOS__SCHEDULER_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace xpcc
{
	namespace freertos
	{
		/**
		 * \brief	
		 * 
		 * vTaskEndScheduler() will not work for the STM32 port of FreeRTOS
		 * therefore no stop() method is implemented here. See the README
		 * file in ext/freertos/port for more details.
		 * 
		 * \ingroup	freertos
		 */
		class Scheduler
		{
		public:
			/**
			 * \brief	Starts the real time kernel
			 * 
			 * This function will never return.
			 * 
			 * The idle task is created automatically when schedule() is called.
			 */
			static inline void
			schedule()
			{
				vTaskStartScheduler();
			}
			
			/**
			 * \brief	Suspends all real time kernel activity while keeping
			 * 			interrupts (including the kernel tick) enabled.
			 * 
			 * After calling suspend() the calling task will continue
			 * to execute without risk of being swapped out until a call to
			 * resume() has been made.
			 * 
			 * API functions that have the potential to cause a context switch
			 * (for example, delay()) must not be called while the scheduler
			 * is suspended.
			 * 
			 * \see		resumeAll()
			 */
			static inline void
			suspend()
			{
				vTaskSuspendAll();
			}
			
			/**
			 * \brief	Resumes real time kernel activity following a call
			 * 			to suspend()
			 * 
			 * @return	If resuming the scheduler caused a context switch then 
			 * 			\c true is returned, otherwise \c false.
			 */
			static inline bool
			resume()
			{
				return (xTaskResumeAll() == pdTRUE);
			}
			
			/// The count of ticks since Scheduler::schedule() was called
			static inline portTickType
			getTicks()
			{
				return xTaskGetTickCount();
			}
			
		};
	}
}

#endif // XPCC_FREERTOS__SCHEDULER_HPP
