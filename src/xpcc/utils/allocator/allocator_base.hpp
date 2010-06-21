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

#ifndef XPCC_ALLOCATOR__ALLOCATOR_BASE_HPP
#define XPCC_ALLOCATOR__ALLOCATOR_BASE_HPP

#include <cstddef>

namespace xpcc
{
	namespace allocator
	{
		/**
		 * \brief	Base class for all allocator types
		 * 
		 * \internal
		 * 
		 * \ingroup	allocator
		 * \author	Fabian Greif
		 */
		template <typename T>
		class AllocatorBase
		{
		public:
			/**
			 * \brief	Construct an object
			 * 
			 * Constructs an object of type T (the template parameter) on the
			 * location pointed by p using its copy constructor to initialize
			 * its value to \p value.
			 * 
			 * Notice that this does not allocate space for the element, it
			 * should already be available at p.
			 */
			void
			construct(T* p, const T& value)
			{
				// placement new
				::new((void *) p) T(value);
			}
			
			/**
			 * \brief	Destroy an object
			 * 
			 * Destroys the object of type T (the template parameter) pointed
			 * by p.
			 * 
			 * Notice that this does not deallocate space for the element.
			 */
			void
			destroy(T* p)
			{
				p->~T();
			}
			
		protected:
			AllocatorBase()
			{
			}
		};
	}
}

#endif // XPCC_ALLOCATOR__ALLOCATOR_BASE_HPP
