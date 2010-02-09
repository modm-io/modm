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
 *
 * $Id: container.hpp 94 2009-10-19 18:44:57Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_ATOMIC__CONTAINER_HPP
#define	XPCC_ATOMIC__CONTAINER_HPP

#include "lock.hpp"

namespace xpcc
{
	namespace atomic
	{
		/**
		 * \brief	Atomic access to objects
		 * 
		 * Example: 
		 * \code
		 * atomic::Containter<uint32_t> data;
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
		 * \warning	This class should be used with precaution because the
		 * 			objects are copied for every access.
		 * \ingroup	atomic
		 */
		template<typename T>
		class Container
		{
		public:
			Container()
			{
			}
			
			inline void
			set(T value)
			{
				Lock lock;
				object = value;
			}
			
			inline T
			get()
			{
				Lock lock;
				return object;
			}
			
			inline T
			swap(T value)
			{
				Lock lock;
				
				T oldValue = object;
				object = value;
				
				return oldValue;
			}
			
			/// \brief		Get direct access to the object
			///
			/// \warning	If the object is access throu this function the
			///				operations are not atomic!
			inline volatile T&
			getObject()
			{
				return object;
			}
		
		private:
			Container(const Container&);	// TODO
			
			Container&
			operator=(const Container&);	// TODO
			
			volatile T object;
		};
	}
}

#endif	// XPCC_ATOMIC__CONTAINER_HPP
