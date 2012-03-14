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

#ifndef	XPCC_ATOMIC__CONTAINER_HPP
#define	XPCC_ATOMIC__CONTAINER_HPP

#include "lock.hpp"
#include <xpcc/architecture/driver/accessor.hpp>

namespace xpcc
{
	namespace atomic
	{
		/**
		 * \brief	Atomic access to objects
		 * 
		 * The atomic container wraps objects and provides atomic access to
		 * them. This comes in handy when simple objects are accessed by an
		 * interrupt and the main program. The container provides secure
		 * access without much work in this case.
		 * 
		 * \warning	This class should be used with precaution because the
		 * 			objects are copied for every access.
		 * 
		 * Example: 
		 * \code
		 * xpcc::atomic::Containter<uint32_t> data;
		 * 
		 * // interrupt routine
		 * ISR() {
		 *     data.set(123);
		 * }
		 * 
		 * function() {
		 *     uint32_t localData = data.get();
		 *     ...
		 * }
		 * \endcode
		 * 
		 * If the interrupt routine is not interruptible (this is the default
		 * for the ATmega, but not for the ATxmega!) no atomic access is needed
		 * then:
		 * \code
		 * xpcc::atomic::Containter<uint32_t> data;
		 * 
		 * ISR() {
		 *     data.directAccess() = 123;
		 * }
		 * \endcode
		 * 
		 * This will slightly faster because no lock needs to be created.
		 * 
		 * If the object is big or a lot of accessed one after another are
		 * done, consider dropping this class and create critical sections
		 * for the access by yourself with xpcc::atomic::Lock.
		 * 
		 * This can be a lot faster because the object don't need to be copied
		 * as it is the case here!
		 * 
		 * \author	Fabian Greif
		 * \ingroup	atomic
		 */
		template<typename T>
		class Container
		{
		public:
			Container(const T& data = T()) :
				object(data)
			{
			}
			
			inline void
			set(T value)
			{
				Lock lock;
				this->object = value;
			}
			
			inline T
			get()
			{
				Lock lock;
				return accessor::asVolatile(this->object);
			}
			
			/**
			 * \brief	Write a new value to the wrapped object and return
			 * 			the old value.
			 * 
			 * This function is needed quite often when exchanging flags
			 * between interrupt routines and the main-loop. 
			 * 
			 * \code
			 * xpcc::atomic::Container<uint8_t> isrCounter;
			 * 
			 * ISR() {
			 *     ...
			 *     isrCounter.directAccess() += 1;
			 * }
			 * 
			 * function() {
			 *     static uint16_t counter;
			 *     counter += isrCounter.swap(0);
			 *     ...
			 * }
			 * \endcode
			 * 
			 * The same behavior for boolean values is provide by
			 * xpcc::atomic::Flag()
			 */
			inline T
			swap(T value)
			{
				Lock lock;
				
				T oldValue = accessor::asVolatile(this->object);
				this->object = value;
				
				return oldValue;
			}
			
			/**
			 * \brief		Get direct access to the object
			 * 
			 * \return		Reference to the object
			 * \warning		If the object is accessed through this function the
			 *				operations are not atomic! This might be useful
			 *				inside a interrupt context where no atomic
			 *				access is necessary.
			 */
			inline T&
			directAccess()
			{
				return this->object;
			}
		
		private:
			Container(const Container&);
			
			Container&
			operator = (const Container&);
			
			T object;
		};
	}
}

#endif	// XPCC_ATOMIC__CONTAINER_HPP
