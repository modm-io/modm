/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, 2016, 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <stdint.h>
#include <cstddef>
#include <type_traits>
#include <modm/architecture/utils.hpp>


namespace modm
{

/**
 * Memory allocator.
 *
 * @tparam	BLOCK_SIZE
 * 		Size of one allocatable block in words (sizeof(T) bytes)
 *		(BLOCKSIZE * sizeof(T) * n) - 4 has to be dividable by 4 for every n
 *
 * @author	Fabian Greif
 * @ingroup modm_driver_block_allocator
 */
template <typename T, unsigned int BLOCK_SIZE >
class BlockAllocator
{
	using SignedType = std::make_signed_t<T>;
public:
	/**
	 * Initialize the raw memory.
	 *
	 * Needs to called before any calls to allocate() or free(). Must
	 * be called only once!
	 *
	 * @param	heapStart
	 * 		Needs to point to the first available byte
	 * @param	heapEnd
	 * 		Needs to point directly above the last available memory
	 * 		position.
	 */
	void
	initialize(void * heapStart, void * heapEnd);

	/// Allocate memory
	void *
	allocate(std::size_t requestedSize);

	/**
	 * Free memory in O(1)
	 *
	 * @param	ptr
	 * 		Must be the same pointer previously acquired by
	 * 		allocate().
	 */
	void
	free(void *ptr);

public:
	std::size_t
	getAvailableSize() const;

private:
	// Align the pointer to a multiple of MODM_ALIGNMENT
	T *
	alignPointer(void * ptr) const;

	//static const int MAX_BLOCK_PARTS = 2048;

	T* start;
	T* end;
	T* freeHint;
};

} // namespace modm

#include "block_allocator_impl.hpp"
