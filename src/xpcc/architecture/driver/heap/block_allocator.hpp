/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2016, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BLOCK_ALLOCATOR_HPP
#define MODM_BLOCK_ALLOCATOR_HPP

#include <stdint.h>
#include <cstddef>

#include <modm/architecture/utils.hpp>
#include <modm/utils/arithmetic_traits.hpp>

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

namespace modm
{
	template <typename T, unsigned int BLOCK_SIZE >
	class BlockAllocator
	{
		typedef typename modm::ArithmeticTraits<T>::SignedType SignedType;
		
	public:
		/**
		 * Initialize the raw memory.
		 * 
		 * Needs to called before any calls to allocate() or free(). Must
		 * be called only once!
		 * 
		 * \param	heapStart
		 * 		Needs to point to the first available byte
		 * \param	heapEnd
		 * 		Needs to point directly above the last available memory
		 * 		position.
		 */
		modm_always_inline void
		initialize(void * heapStart, void * heapEnd);
		
		/**
		 * Allocate memory
		 * 
		 */
		modm_always_inline void *
		allocate(std::size_t requestedSize);
		
		/**
		 * Free memory in O(1)
		 * 
		 * \param	ptr
		 * 		Must be the same pointer previously acquired by
		 * 		allocate().
		 */
		modm_always_inline void
		free(void *ptr);
		
	public:
		std::size_t
		getAvailableSize() const;
		
	private:
		// Align the pointer to a multiple of MODM_ALIGNMENT
		modm_always_inline T *
		alignPointer(void * ptr) const;
		
		//static const int MAX_BLOCK_PARTS = 2048;
		
		T* start;
		T* end;
		
		T* freeHint;
	};
}

#include "block_allocator_impl.hpp"

#endif	// MODM_BLOCK_ALLOCATOR_HPP
