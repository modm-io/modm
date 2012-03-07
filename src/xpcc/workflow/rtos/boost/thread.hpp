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
 * $Id: thread.hpp 703 2012-01-20 22:40:22Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_BOOST__THREAD_HPP
#define XPCC_BOOST__THREAD_HPP

#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>

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
 * 
 * \hideinitializer
 * \ingroup	boost_rtos
 */
/*#define	TIME_LOOP(frequency)										\
		for(portTickType lastTime = xTaskGetTickCount() ;			\
			vTaskDelayUntil(&lastTime,								\
					static_cast<portTickType>(frequency)), true ;	\
			)*/

/**
 * \brief	Convert between scheduler ticks and milliseconds
 * 
 * For example 20 ms in scheduler ticks might be written as:
 * \code
 * 20 * MILLISECONDS
 * \endcode
 * 
 * \hideinitializer
 * \ingroup	boost_rtos
 */
#define	MILLISECONDS		1

namespace xpcc
{
	namespace rtos
	{
		// forward declaration
		class Scheduler;
		
		/**
		 * \brief	Thread
		 * 
		 * \ingroup	boost_rtos
		 */
		class Thread
		{
		public:
			/**
			 * \brief	Create a Thread
			 * 
			 * \param	priority	unused for boost::thread
			 * \param	stackDepth	unused for boost::thread
			 * \param	name		unused for boost::thread
			 * 
			 * \warning	Threads may not be created while the scheduler is running!
			 * 			Create them be before calling Scheduler::schedule() or
			 * 			stop the scheduler and restart it afterwards.
			 */
			Thread(uint32_t priority = 0,
					uint32_t stackDepth = 0,
					const char* name = NULL);
			
			/// Delete the thread
			virtual ~Thread();
			
			/// Obtain the priority of the thread
			uint_fast32_t
			getPriority() const
			{
				return 0;
			}
			
			/**
			 * \brief	Set the priority of the thread
			 * 
			 * Does nothing for boost::thread.
			 */
			void
			setPriority(uint_fast32_t priority)
			{
				// avoid compiler warnings
				(void) priority;
			}
			
			/**
			 * If a thread wishes to avoid being interrupted, it can create an
			 * instance of Lock. Objects of this class disable interruption
			 * for the thread that created them on construction, and restore
			 * the interruption state to whatever it was before on destruction.
			 * 
			 * \see		boost::this_thread::disable_interruption
			 */
			typedef boost::this_thread::disable_interruption Lock;
			
		protected:
			/**
			 * \brief	Delay for the number of Milliseconds
			 */
			static inline void
			sleep(uint32_t ms)
			{
				boost::this_thread::sleep(boost::posix_time::milliseconds(ms));
			}
			
			/**
			 * \brief	Force a context switch
			 * 
			 * Gives control to other threads ready to run.
			 */
			static inline void
			yield()
			{
				boost::this_thread::yield();
			}
			
			/**
			 * \brief	Worker function
			 * 
			 * Must be implemented to never return (i.e. continuous loop)
			 */
			virtual void
			run() = 0;
			
		private:
			friend class Scheduler;
			
			// start the execution of the thread
			void
			start();
			
			Thread *next;
			static Thread* head;
			
			boost::mutex mutex;
			boost::scoped_ptr<boost::thread> thread;
		};
	}
}

#endif // XPCC_BOOST__THREAD_HPP
