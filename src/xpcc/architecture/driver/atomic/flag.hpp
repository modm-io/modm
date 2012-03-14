// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

#ifndef	XPCC_ATOMIC__FLAG_HPP
#define	XPCC_ATOMIC__FLAG_HPP

#include "lock.hpp"

namespace xpcc
{
	namespace atomic
	{
		/**
		 * \brief		Flag to signal events between interrupts and the 
		 *				main-loop
		 * 
		 * This class is quite handy when exchanging flags between a interrupt
		 * routine and the main-loop. 
		 * 
		 * \code
		 * xpcc::atomic::Flag isrFlag;
		 * 
		 * ISR() {
		 *     ...
		 *     isrFlag.set();
		 * }
		 * 
		 * function() {
		 *     bool flag = isrFlag.testAndSet(false);
		 *     ...
		 * }
		 * \endcode
		 * 
		 * \ingroup	atomic
		 * \author	Fabian Greif
		 */
		class Flag
		{
		public:
			Flag(bool state = false);
			
			Flag(const Flag& other);
			
			Flag&
			operator = (const Flag& other);
			
			/**
			 * \brief	Check state of the flag
			 */
			inline bool
			test() const
			{
				return this->state;
			}
			
			/**
			 * \brief	Set flag
			 */
			inline void
			set()
			{
				this->state = true;
			}
			
			/**
			 * \brief	Clear flag
			 */
			inline void
			reset()
			{
				this->state = false;
			}
			
			/**
			 * \brief	Sets the flag to a new value and returns the old one
			 */
			inline bool
			testAndSet(bool value)
			{
				Lock lock;
				
				bool oldValue = this->state;
				this->state = value;
				
				return oldValue;
			}
		
		private:
			volatile bool state;
		};
	}
}

#endif	// XPCC_ATOMIC__FLAG_HPP
