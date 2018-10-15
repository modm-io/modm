/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, 2014-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PT_THREAD_HPP
#define MODM_PT_THREAD_HPP

#include <stdint.h>
#include "macros.hpp"

namespace modm
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
		 * For other examples take a look in the \c examples folder in the MODM
		 * root folder.
		 *
		 * \warning	The names \c ptState and \c ptYield are reserved and may not
		 * 			be used as variables or function names!
		 *
		 * \ingroup	modm_processing_protothread
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

			/// @cond
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

#endif // MODM_PT_THREAD_HPP
