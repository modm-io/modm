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

#ifndef XPCC__TIMEOUT_HPP
#define XPCC__TIMEOUT_HPP

#include <xpcc/architecture/general/time/clock.hpp>

#include "timestamp.hpp"

namespace xpcc
{
	/**
	 * \brief		Software timer
	 *
	 * Has to be polled to check if it has expired.
	 * 
	 * \tparam	T	Used timer, default is xpcc::Clock() which has 
	 * 				a millisecond resolution.
	 * 
	 * Usage:
	 * \include	timeout.cpp
	 * 
	 * Complete example for the ATMega644:
	 * \include timeout/main.cpp
	 * 
	 * \author	Fabian Greif
	 * \ingroup	workflow
	 */
	template<typename T = ::xpcc::Clock>
	class Timeout
	{
	public:
		Timeout();
		
		/**
		 * \brief	Create and start the timeout
		 */
		Timeout(const Timestamp time);
		
		/**
		 * \brief	Check if the given time has passed.
		 * 
		 * If isExpired() changes to \c true, it will keep this value till
		 * a call of restart().
		 */
		bool
		isExpired();
		
		/**
		 * \brief	Stop the timer
		 * 
		 * Sets isExpired() to true.
		 */
		inline void
		stop();
		
		/**
		 * \brief	Set a new timeout value.
		 */
		void
		restart(Timestamp time);
		
	private:
		Timestamp endTime;
		enum {
			ACTIVE,
			EXPIRED
		} state;
	};
}

#include "timeout_impl.hpp"

#endif // XPCC__TIMEOUT_HPP
