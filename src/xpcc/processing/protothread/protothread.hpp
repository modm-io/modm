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

#ifndef XPCC_PT__THREAD_HPP
#define XPCC_PT__THREAD_HPP

#include <stdint.h>
#include "macros.hpp"

namespace xpcc
{
	namespace pt
	{
		/**
		 * \brief	A very lightweight, stackless thread
		 *
		 * Because protothreads do not save the stack context across a blocking
		 * call, local variables are not preserved when the protothread blocks.
		 * This means that local variables should be used with utmost care - if in
		 * doubt, do not use local variables inside a protothread! Use
		 * private/protected member variables to save state between a context switch.
		 *
		 * A protothread is driven by repeated calls to the run()-function in which
		 * the protothread is running. Each time the function is called, the
		 * protothread will run until it blocks or exits. Thus the scheduling of
		 * protothreads is done by the application that uses protothreads.
		 *
		 * Example:
		 * \code
		 * #include <xpcc/architecture.hpp>
		 * #include <xpcc/utils/protothread.hpp>
		 * #include <xpcc/utils/timeout.hpp>
		 *
		 * typedef GpioOutputB0 Led;
		 *
		 * class BlinkingLight : public xpcc::pt::Protothread
		 * {
		 * public:
		 *     bool
		 *     run()
		 *     {
		 *         PT_BEGIN();
		 *
		 *         // set everything up
		 *         Led::setOutput();
		 *         Led::set();
		 *
		 *         while (true)
		 *         {
		 *             timeout.restart(100);
		 *             Led::set();
		 *             PT_WAIT_UNTIL(timeout.isExpired());
		 *
		 *             timeout.restart(200);
		 *             Led::reset();
		 *             PT_WAIT_UNTIL(timeout.isExpired());
		 *         }
		 *
		 *         PT_END();
		 *     }
		 *
		 * private:
		 *     xpcc::ShortTimeout timeout;
		 * };
		 *
		 *
		 * ...
		 * BlinkingLight light;
		 *
		 * while (...) {
		 *     light.run();
		 * }
		 * \endcode
		 *
		 * For other examples take a look in the \c examples folder in the XPCC
		 * root folder.
		 *
		 * \warning	The names \c ptState and \c ptYield are reserved and may not
		 * 			be used as variables or function names!
		 *
		 * \ingroup	protothread
		 */
		class Protothread
		{
		public:
			/**
			 * \brief	Construct a new protothread that will start from the
			 * 			beginning of its run() function.
			 */
			Protothread() :
				ptState(0)
			{
			}

			/// Restart protothread.
			inline void
			restart()
			{
				this->ptState = 0;
			}

			/**
			 * \brief	Stop the protothread from running.
			 *
			 * Happens automatically at PT_END.
			 *
			 * \note	This differs from the Dunkels' original protothread
			 * 			behavior (his restart automatically, which is usually not
			 * 			what you want).
			 */
			inline void
			stop()
			{
				this->ptState = Invalid;
			}

			/**
			 * \brief	Check if the protothread is still running
			 *
			 * \return	\c true if the protothread is running or waiting,
			 * 			\c false if it has ended or exited.
			 */
			inline bool
			isRunning() const
			{
				return (this->ptState != Invalid);
			}

#ifdef __DOXYGEN__
			/// @cond
			/**
			 * \brief	Run the protothread
			 *
			 * Run next part of protothread or return immediately if it's still
			 * waiting. Returns \c true if protothread is still running, \c false
			 * if it has finished.
			 *
			 * Implement this method in your Protothread subclass.
			 *
			 * \warning	This is method is not virtual, therefore you cannot access
			 * 			it through a Pointer to this class, but only directly from
			 * 			the subclass! This was done on purpose to keep the memory
			 * 			footprint low.
			 */
			bool
			run();
#endif

		protected:
			/**
			 * Used to store a protothread's position (what Dunkels calls a
			 * "local continuation").
			 */
			typedef uint16_t PtState;

			/// An invalid line number, used to mark the protothread has ended.
			static const PtState Invalid = static_cast<PtState>(-1);

			/**
			 * Stores the protothread's position (by storing the line number of
			 * the last PT_WAIT, which is then switched on at the next Run).
			 */
			PtState ptState;
			/// @endcond
		};
	}
}

#endif // XPCC_PT__THREAD_HPP
