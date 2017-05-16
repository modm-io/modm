/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_ACCESSOR_RAM_HPP
#define	MODM_ACCESSOR_RAM_HPP

#include <cstddef>
#include <modm/architecture/utils.hpp>

namespace modm
{
	namespace accessor
	{
		/**
		 * \brief	Pointer to RAM
		 * 
		 * \ingroup	accessor
		 * \author	Fabian Greif
		 */
		template<typename T>
		class Ram
		{
		public:
			modm_always_inline
			Ram(const T* addr = 0) :
				address(addr)
			{
			}
			
			template <typename U>
			modm_always_inline
			explicit Ram(const Ram<U>& rhs) :
				address((T*) rhs.address)
			{
			}
			
			modm_always_inline
			const T
			operator *() const
			{
				return *address;
			}
			
			modm_always_inline
			const T
			operator [](std::size_t index) const
			{
				return *(address + index);
			}
			
			modm_always_inline
			Ram&
			operator ++ ()
			{
				*this += 1;
				return *this;
			}

			modm_always_inline
			Ram
			operator ++ (int)
			{
				Ram ret = *this;
				++*this;
				return ret;
			}

			modm_always_inline
			Ram&
			operator -- ()
			{
				*this -= 1;
				return *this;
			}

			modm_always_inline
			Ram&
			operator -- (int)
			{
				Ram ret = *this;
				--*this;
				return ret;
			}
			
			modm_always_inline
			Ram&
			operator += (std::size_t rhs)
			{
				address += rhs;
				return *this;
			}
			
			modm_always_inline
			Ram&
			operator -= (std::size_t rhs)
			{
				address -= rhs;
				return *this;
			}
			
			modm_always_inline
			const T*
			getPointer() const
			{
				return address;
			}
		
		private:
			const T* address;
		};
	}
}

#endif	// MODM_ACCESSOR_RAM_HPP
