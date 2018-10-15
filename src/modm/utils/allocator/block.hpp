/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ALLOCATOR_BLOCK_HPP
#define MODM_ALLOCATOR_BLOCK_HPP

#include "allocator_base.hpp"

namespace modm
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
		 * \ingroup	modm_utils_allocator
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
			Block() :
				AllocatorBase<T>()
			{
			}

			Block(const Block& other) :
				AllocatorBase<T>(other)
			{
			}

			template <typename U>
			Block(const Block<U, BLOCKSIZE>&) :
				AllocatorBase<T>()
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

#endif // MODM_ALLOCATOR_BLOCK_HPP
