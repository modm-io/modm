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

#ifndef MODM_ALLOCATOR_DYNAMIC_HPP
#define MODM_ALLOCATOR_DYNAMIC_HPP

#include "allocator_base.hpp"

namespace modm
{
	namespace allocator
	{
		/**
		 * \brief	Dynamic memory allocator
		 *
		 * Wrapper for the underlying memory management. No additional
		 * management is done.
		 *
		 * \ingroup	modm_utils_allocator
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

#endif // MODM_ALLOCATOR_DYNAMIC_HPP
