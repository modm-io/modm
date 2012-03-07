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
 * $Id: dynamic.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ALLOCATOR__DYNAMIC_HPP
#define XPCC_ALLOCATOR__DYNAMIC_HPP

#include "allocator_base.hpp"

namespace xpcc
{
	namespace allocator
	{
		/**
		 * \brief	Dynamic memory allocator
		 * 
		 * Wrapper for the underlying memory management. No additional
		 * management is done.
		 * 
		 * \ingroup	allocator
		 * \author	Fabian Greif
		 */
		template <typename T>
		class Dynamic : public AllocatorBase<T>
		{
		public:
			template <typename U>
			struct rebind
			{
				typedef Dynamic<U> other;
			};
			
		public:
			Dynamic() :
				AllocatorBase<T>()
			{
			}
			
			Dynamic(const Dynamic& other) :
				AllocatorBase<T>(other)
			{
			}
			
			template <typename U>
			Dynamic(const Dynamic<U>&) :
				AllocatorBase<T>()
			{
			}
			
			T*
			allocate(size_t n)
			{
				// allocate the memory without calling the constructor
				// of the associated data-type.
				return static_cast<T*>(::operator new(n * sizeof(T)));
			}
			
			void
			deallocate(T* p)
			{
				// it is important to use this form here, otherwise the
				// destructor of p will be called which is unwanted here.
				// The destructor can be called with the destroy()-method.
				::operator delete(p);
			}
		};
	}
}

#endif // XPCC_ALLOCATOR__DYNAMIC_HPP
