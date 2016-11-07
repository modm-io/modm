/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_FREERTOS__SCHEDULER_HPP
#define XPCC_FREERTOS__SCHEDULER_HPP

#ifndef XPCC_RTOS__SCHEDULER_HPP
#	error "Don't include this file directly, use <xpcc/processing/rtos/scheduler.hpp>"
#endif

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
			 * Starts the SysTick Timer (1ms period) and attaches to its
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
