/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ALLOCATOR_STATIC_HPP
#define MODM_ALLOCATOR_STATIC_HPP

#include "allocator_base.hpp"

namespace modm
{
	namespace allocator
	{
		/**
		 * \brief	Static memory allocator
		 *
		 * Allocates a big static block and distributes pieces of it during
		 * run-time. No reallocation is done when no more pieces are available.
		 *
		 * \ingroup	modm_utils_allocator
		 * \author	Fabian Greif
		 */
		template <typename T,
				  std::size_t N>
		class Static : public AllocatorBase<T>
		{
		public:
			template <typename U>
			struct rebind
			{
				typedef Static<U, N> other;
			};

		public:
			Static() :
				AllocatorBase<T>()
			{
			}

			Static(const Static& other) :
				AllocatorBase<T>(other)
			{
			}

			template <typename U>
			Static(const Static<U, N>&) :
				AllocatorBase<T>()
			{
			}

			// TODO
			T*
			allocate(std::size_t other);

			void
			deallocate(T*);

		private:
			T memory[N];
		};
	}
}

#endif // MODM_ALLOCATOR_STATIC_HPP
