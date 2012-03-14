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

#ifndef XPCC_FREERTOS__SCHEDULER_HPP
#define XPCC_FREERTOS__SCHEDULER_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace xpcc
{
	namespace rtos
	{
		/**
		 * FreeRTOS Scheduler	
		 * 
		 * <h2>Interrupt Priorities</h2>
		 * 
		 * The STM32 implements only four priority bits. The lower four bits of
		 * the eight bit priority register are unused. A lower value means a
		 * higher priority.
	 	 * Therefore 0 (=0) is the highest priority and 15 (=15 << 4 => 0xf0)
	 	 * is the lowest priority.
		 * 
		 * The Kernel has the lowest possible priority 15 to make sure that it
		 * is not interrupting any other interrupt. During the execution of
		 * an API function the priority is raised to 11.
		 * 
		 * Any interrupt that calls an API function must have a priority
		 * between 15 and 11, otherwise the system might crash.
		 *
		 * Interrupts that do not use any FreeRTOS API functions can use any
		 * interrupt priority. If the priority is higher (lower value) than 11
		 * the execution won't be delayed by anything the kernel is doing.
		 * 
		 * 
		 * \warning	vTaskEndScheduler() will not work for the STM32 port of
		 * 			FreeRTOS therefore no stop() method is implemented here.
		 * 			See the README file in ext/freertos/port for more details.
		 * 
		 * \ingroup	freertos
		 */
		class Scheduler
		{
		public:
			/**
			 * \brief	Starts the real time kernel
			 * 
			 * Starts the SysTick Timer (1ms periode) and attaches to its
			 * interrupt.
			 * 
			 * The idle thread is created automatically when schedule() is called.
			 * 
			 * \warning	This function will never return.
			 */
			static inline void
			schedule()
			{
				vTaskStartScheduler();
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
