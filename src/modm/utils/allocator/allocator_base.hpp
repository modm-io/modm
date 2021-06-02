/*
 * Copyright (c) 2009-2010, Fabian Greif
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

#ifndef MODM_ALLOCATOR_ALLOCATOR_BASE_HPP
#define MODM_ALLOCATOR_ALLOCATOR_BASE_HPP

#include <cstddef>
#include <new>		// needed for placement new

namespace modm
{
	namespace allocator
	{
		/**
		 * \brief	Base class for all allocator types
		 *
		 * \internal
		 *
		 * \ingroup	modm_utils_allocator
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
			static inline void
			construct(T* p, const T& value)
			{
				if (p == nullptr) return;
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
			static inline void
			destroy(T* p)
			{
				if (p == nullptr) return;
				p->~T();
			}

		protected:
			AllocatorBase()
			{
			}
		};
	}
}

#endif // MODM_ALLOCATOR_ALLOCATOR_BASE_HPP
