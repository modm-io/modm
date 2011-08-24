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

#ifndef XPCC_RTOS__TASK_HPP
#define XPCC_RTOS__TASK_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace xpcc
{
	namespace rtos
	{
		/**
		 * \brief	
		 * 
		 * \ingroup	rtos
		 */
		class Task
		{
		public:
			/**
			 * \brief	Create a task
			 * 
			 * \warning	Tasks may not be created while the scheduler is running!
			 * 			Create them be before calling Scheduler::schedule() or
			 * 			stop the scheduler and restart it afterwards.
			 */
			Task(const char* name = NULL,
					unsigned short stackDepth = configMINIMAL_STACK_SIZE,
					unsigned portBASE_TYPE priority = tskIDLE_PRIORITY + 1);
			
			virtual ~Task();
			
			/// Obtain the priority of the task
			unsigned portBASE_TYPE
			getPriority();
			
			void
			setPriority(unsigned portBASE_TYPE priority);
			
			
			void
			suspend();
			
			/// Must not be called from within an interrupt
			void
			resume();
			
			void
			resumeFromInterrupt();
			
			/**
			 * \brief	Suspends all real time kernel activity while keeping
			 * 			interrupts (including the kernel tick) enabled.
			 * 
			 * After calling suspendAll() the calling task will continue
			 * to execute without risk of being swapped out until a call to
			 * resumeAll() has been made.
			 * 
			 * API functions that have the potential to cause a context switch
			 * (for example, delay()) must not be called while the scheduler
			 * is suspended.
			 * 
			 * \see		resumeAll()
			 */
			void
			suspendAll();
			
			/// Resumes real time kernel activity following a call to suspendAll()
			void
			resumeAll();
			
		protected:
			static inline void
			delay(portTickType ticks)
			{
				vTaskDelay(ticks);
			}
			
			// TODO
			//delayUntil();
			
			static inline void
			yield()
			{
				taskYIELD();
			}
			
			/**
			 * Must be implemented to never return (i.e. continuous loop)
			 */
			virtual void
			run() = 0;
			
		private:
			static void
			wrapper(void *object);
			
			xTaskHandle handle;
		};
	}
}

#endif // XPCC_RTOS__TASK_HPP
