/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2018, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_FREERTOS_THREAD_HPP
#define MODM_FREERTOS_THREAD_HPP

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * \brief	Create a timed periodic loop
 * 
 * Example:
 * \code
 * void
 * Thread::run()
 * {
 *     TIME_LOOP(20 * MILLISECONDS)
 *     {
 *         // will be executed every 20ms
 *     }
 * }
 * \endcode
 * 
 * \param	frequency	Frequency in ticks
 * 
 * \see		MILLISECONDS
 * \see		vTaskDelayUntil() from FreeRTOS
 * 
 * \hideinitializer
 * \ingroup	freertos
 */
#define	TIME_LOOP(frequency)										\
		for(portTickType lastTime = xTaskGetTickCount() ;			\
			vTaskDelayUntil(&lastTime,								\
					static_cast<portTickType>(frequency)), true ;	\
			)

/**
 * \brief	Convert between scheduler ticks and milliseconds
 * 
 * For example 20 ms in scheduler ticks might be written as:
 * \code
 * 20 * MILLISECONDS
 * \endcode
 * 
 * This should only be used with constant values as the calculation is done
 * using floating point. For constant values the compiler will do the calculation
 * at compile time.
 * 
 * For non constant value use the following formula:
 * \code
 * static_cast<portTickType>((time * configTICK_RATE_HZ) / 1000)
 * \endcode
 * The parentheses are important because otherwise the division might be done
 * first which will lead to wrong results.
 * 
 * \hideinitializer
 * \ingroup	freertos
 */
#define	MILLISECONDS		(configTICK_RATE_HZ / 1000.0)

namespace modm
{
	namespace rtos
	{
		/**
		 * \brief	Thread
		 * 
		 * <h2>FreeRTOS suspend()/resume()</h2>
		 * 
		 * suspend() and resume() are dangerous and are therefore not available.
		 * 
		 * Example:
		 * thread 1 
		 *  1) lock mutex M 
		 *  2) do something 
		 *  3) unlock mutex 
		 * 
		 * thread 2 
		 *  1) suspend thread 1 
		 *  2) lock mutex 
		 *  3) unlock mutex M 
		 *  4) resume thread 1
		 * 
		 * If 2.1 happens during 1.2 the first flow for ever brakes, and the
		 * second - is for ever tired of waiting.
		 * 
		 * Use modm::rtos::Semaphore if you need to suspend the execution
		 * of a thread.  
         * 
         * Sideeffects: 
         * Thread constructor disables the SysTickTimer until the Scheduler is called.
		 * 
		 * \ingroup	freertos
		 */
		class Thread
		{
			static const uint16_t minimalStackSize = 200;
			
		public:
			/**
			 * \brief	Create a Thread
			 * 
			 * \param	priority	Priority (default is 0)
			 * \param	stackDepth	Stack size for the thread in bytes
			 * \param	name		Name of the thread (only used for debugging,
			 * 						can be left empty)
			 * 
			 * \warning	Threads may not be created while the scheduler is running!
			 * 			Create them be before calling Scheduler::schedule() or
			 * 			stop the scheduler and restart it afterwards.
			 */
			Thread(uint32_t priority = 0,
					uint16_t stackDepth = minimalStackSize,
					const char* name = NULL);
			
			/// Delete the thread
			virtual ~Thread();
			
			/// Obtain the priority of the thread
			uint32_t
			getPriority() const;
			
			/**
			 * \brief	Set the priority of the thread
			 * 
			 * Might cause a context switch if the priority is set to lower
			 * value than the highest priority of a thread ready to run.
			 */
			void
			setPriority(uint32_t priority);
			
			/**
			 * \brief	When created suspends all real time kernel activity
			 * 			while keeping interrupts (including the kernel tick)
			 * 			enabled.
			 * 
			 * After creating a instance the calling thread will continue
			 * to execute without risk of being swapped out until the destruction
			 * of the lock instance.
			 * 
			 * API functions that have the potential to cause a context switch
			 * (for example, delay()) must not be called while the scheduler
			 * is suspended.
			 */
			class Lock
			{
			public:
				Lock()
				{
					vTaskSuspendAll();
				}
				
				~Lock()
				{
					xTaskResumeAll();
				}
			};
			
		protected:
			/**
			 * \brief	Delay for the number of ticks
			 * 
			 * Use the MILLISECONDS macro to convert ticks to milliseconds:
			 * \code
			 * sleep(10 * MILLISECONDS);
			 * \endcode 
			 * 
			 * \param	ticks	Number of scheduler ticks to delay for
			 * \see		MILLISECONDS
			 */
			static inline void
			sleep(portTickType ticks)
			{
				vTaskDelay(ticks);
			}
			
			// TODO
			//sleepUntil();
			
			/**
			 * \brief	Force a context switch
			 * 
			 * Gives control to other threads ready to run.
			 */
			static inline void
			yield()
			{
				taskYIELD();
			}
			
			/**
			 * \brief	Worker function
			 * 
			 * Must be implemented to never return (i.e. continuous loop)
			 */
			virtual void
			run() = 0;
			
		private:
			static void
			wrapper(void *object);
			
			// disable copy constructor
			Thread(const Thread& other);
			
			// disable assignment operator
			Thread&
			operator = (const Thread& other);
			
			xTaskHandle handle;
		};
	}
}

#endif // MODM_FREERTOS_THREAD_HPP
