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
/*
 * Protothread class and macros for lightweight, stackless threads in C++.
 *
 * This was "ported" to C++ from Adam Dunkels' protothreads C library at:
 *     http://www.sics.se/~adam/pt/
 *
 * Originally ported for use by Hamilton Jet (www.hamiltonjet.co.nz) by
 * Ben Hoyt, but stripped down for public release. See his blog entry about
 * it for more information:
 *     http://blog.micropledge.com/2008/07/protothreads/
 */

#ifndef XPCC__PROTOTHREAD_HPP
#define XPCC__PROTOTHREAD_HPP

#include <stdint.h>
#include <xpcc/utils/macros.hpp>

/**
 * \ingroup		workflow
 * \defgroup	protothread		Protothreads
 * \brief		Lightweight, stackless threads
 * 
 * \see	http://www.sics.se/~adam/pt/
 */

namespace xpcc
{
	/**
	 * \brief	A very lightweight, stackless thread
	 * 
	 * Example:
	 * \code
	 * #include <xpcc/architecture/general/gpio.hpp>
	 * #include <xpcc/workflow/protothread.hpp>
	 * #include <xpcc/workflow/timeout.hpp>
	 * 
	 * GPIO__OUTPUT(LED, B, 0);
	 * 
	 * class BlinkingLight : public xpcc::Protothread
	 * {
	 * public:
	 *     bool
	 *     run()
	 *     {
	 *         PT_BEGIN();
	 *         
	 *         // set everything up
	 *         LED::setOutput();
	 *         LED::set();
	 *         
	 *         while (true)
	 *         {
	 *             this->timer.start(100);
	 *             LED::set();
	 *             PT_WAIT_UNTIL(this->timer.isExpired());
	 *             
	 *             this->timer.start(200);
	 *             LED::reset();
	 *             PT_WAIT_UNTIL(this->timer.isExpired());
	 *         }
	 *         
	 *         PT_END();
	 *     }
	 *     
	 * private:
	 *     xpcc::Timeout<> timer;
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
		// Used to store a protothread's position (what Dunkels calls a
		// "local continuation").
		typedef uint16_t State;
		
		// An invalid line number, used to mark the protothread has ended.
		static const State Invalid = static_cast<State>(-1);
		
		// Stores the protothread's position (by storing the line number of
		// the last PT_WAIT, which is then switched on at the next Run).
		State ptState;
	};
}

#ifdef __DOXYGEN__

/**
 * Declare start of protothread
 * 
 * \warning	Use at start of the run() implementation!
 * \ingroup	protothread
 */
#define PT_BEGIN()

/**
 * Stop protothread and end it
 * 
 * \warning	Use at end of the run() implementation!
 * \ingroup	protothread
 */
#define PT_END()

/**
 * Yield protothread till next call to its run().
 * 
 * \ingroup	protothread
 */
#define PT_YIELD()

/**
 * Cause protothread to wait until given condition is true.
 * 
 * \ingroup	protothread
 */
#define PT_WAIT_UNTIL(condition)


/**
 * Cause protothread to wait until given child protothread completes.
 * 
 * \ingroup	protothread
 */
#define PT_WAIT_THREAD(child)

/**
 * Restart and spawn given child protothread and wait until it completes.
 * 
 * \ingroup	protothread
 */
#define PT_SPAWN(child)

/**
 * Reset protothread to start from the beginning
 * 
 * In the next executing cycle the protothread will restart its execution at
 * its PT_BEGIN.
 * 
 * \ingroup	protothread
 */
#define PT_RESTART()

/**
 * Stop and exit from protothread.
 * 
 * \ingroup	protothread
 */
#define PT_EXIT()

#else	// !__DOXYGEN__

#define PT_BEGIN() \
	bool ptYielded ATTRIBUTE_UNUSED = true; \
	switch (this->ptState) { \
		case 0: 

#define PT_END() \
		default: ; \
	} \
	this->stop(); \
	return false;

#define PT_YIELD() \
    do { \
		ptYielded = false; \
		this->ptState = __LINE__; \
		case __LINE__: \
			if (!ptYielded) \
				return true; \
	} while (0)

#define PT_WAIT_UNTIL(condition) \
    do { \
		this->ptState = __LINE__; \
		case __LINE__: \
			if (!(condition)) \
				return true; \
    } while (0)

#define PT_WAIT_THREAD(child) 	PT_WAIT_UNTIL(!(child).run())

#define PT_SPAWN(child) \
    do { \
		(child).restart(); \
		PT_WAIT_THREAD(child); \
    } while (0)

#define PT_RESTART() \
	do { \
		this->restart(); \
		return true; \
	} while (0)

#define PT_EXIT() \
	do { \
		this->stop(); \
		return false; \
	} while (0)


#endif	// __DOXYGEN__
#endif	// XPCC__PROTOTHREAD_HPP
