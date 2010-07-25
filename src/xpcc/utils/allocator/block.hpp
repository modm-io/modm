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

#ifndef XPCC_ALLOCATOR__BLOCK_HPP
#define XPCC_ALLOCATOR__BLOCK_HPP

#include "allocator_base.hpp"

namespace xpcc
{
	namespace allocator
	{
		/**
		 * \brief	Block allocator
		 * 
		 * Allocates a big block of memory and then distribute small pieces of
		 * it. The memory is not released until the destruction of the
		 * allocator.
		 * If more memory is needed a new block is allocated.
		 * 
		 * This technique is known as "memory pool".
		 * 
		 * \ingroup	allocator
		 * \author	Fabian Greif
		 */
		template <typename T,
				  std::size_t BLOCKSIZE>
		class Block : public AllocatorBase<T>
		{
		public:
			template <typename U>
			struct rebind
			{
				typedef Block<U, BLOCKSIZE> other;
			};
		
		public:
			Block()
			{
			}
			
			Block(const Block&)
			{
			}
			
			template <typename U>
			Block(const Block<U, BLOCKSIZE>&)
			{
			}
			
			// TODO
			T*
			allocate();
			
			void
			deallocate(T*);
		};
	}
}

#endif // XPCC_ALLOCATOR__BLOCK_HPP
