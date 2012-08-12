// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
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

#ifndef XPCC__BLOCK_ALLOCATOR_HPP
#define XPCC__BLOCK_ALLOCATOR_HPP

#include <stdint.h>
#include <xpcc/architecture/utils.hpp>
#include <xpcc/utils/arithmetic_traits.hpp>

/**
 * Memory allocator.
 * 
 * 
 * \tparam	T
 * 		TODO
 * 
 * \tparam	BLOCK_SIZE
 * 		Size of one allocatable block in words (sizeof(T) bytes)
 *		(BLOCKSIZE * sizeof(T) * n) - 4 has to be dividable by 4 for every n
 * 
 * \author	Fabian Greif
 */

namespace xpcc
{
	template <typename T, unsigned int BLOCK_SIZE >
	class BlockAllocator
	{
		typedef typename xpcc::ArithmeticTraits<T>::SignedType SignedType;
		
	public:
		/**
		 * Initialize the raw memory.
		 * 
		 * Needs to called before any calls to allocate() or free(). Must
		 * be called only once!
		 * 
		 * \param	heapStart
		 * 		Needs to point on the first available byte
		 * \param	heapEnd
		 * 		Needs to point directly above the last available memory
		 * 		position.
		 */
		ALWAYS_INLINE void
		initialize(void * heapStart, void * heapEnd);
		
		/**
		 * Allocate memory
		 * 
		 */
		ALWAYS_INLINE void *
		allocate(std::size_t requestedSize);
		
		/**
		 * Free memory in O(1)
		 * 
		 * \param	ptr
		 * 		Must be the same pointer previously acquired by
		 * 		allocate().
		 */
		ALWAYS_INLINE void
		free(void *ptr);
		
	public:
		std::size_t
		getAvailableSize() const;
		
	private:
		// Align the pointer to 
		ALWAYS_INLINE T *
		alignPointer(void * ptr) const;
		
		//static const int MAX_BLOCK_PARTS = 2048;
		
		T* start;
		T* end;
		
		T* freeHint;
	};
}

#include "block_allocator_impl.hpp"

#endif	// XPCC__BLOCK_ALLOCATOR_HPP
