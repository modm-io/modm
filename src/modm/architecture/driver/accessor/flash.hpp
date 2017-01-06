/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_ACCESSOR_FLASH_HPP
#define	MODM_ACCESSOR_FLASH_HPP

#include <modm/architecture/utils.hpp>
#include <modm/io/iostream.hpp>

#include "flash_reader.hpp"

namespace modm
{
	namespace accessor
	{
		/**
		 * \brief	Pointer to flash memory
		 * 
		 * This template can mostly be used like a regular pointer, but operates
		 * on the flash memory rather than RAM. It will automatically read the 
		 * data from flash when dereferenced.
		 * 
		 * Based on a implementation of Rolf Magnus, see
		 * http://www.mikrocontroller.net/topic/78610#656695
		 * 
		 * \ingroup	accessor
		 * \author	Fabian Greif
		 */
		template<typename T>
		class Flash
		{
		public:
			modm_always_inline
			explicit Flash(const T* addr = 0) :
				address(addr)
			{
			}
			
			template <typename U>
			modm_always_inline
			explicit Flash(const Flash<U>& rhs) :
				address((T*) rhs.address)
			{
			}
			
			modm_always_inline
			const T
			operator *() const
			{
				return FlashReader<T, sizeof(T)>::read(address);
			}
			
			modm_always_inline
			const T
			operator [](size_t index) const
			{
				return FlashReader<T, sizeof(T)>::read(address + index);
			}
			
			modm_always_inline
			Flash&
			operator++()
			{
				*this += 1;
				return *this;
			}

			modm_always_inline
			Flash
			operator++(int)
			{
				Flash ret = *this;
				++*this;
				return ret;
			}

			modm_always_inline
			Flash&
			operator--()
			{
				*this -= 1;
				return *this;
			}

			modm_always_inline
			Flash&
			operator--(int)
			{
				Flash ret = *this;
				--*this;
				return ret;
			}
			
			modm_always_inline
			Flash&
			operator+=(size_t rhs)
			{
				address += rhs;
				return *this;
			}
			
			modm_always_inline
			Flash&
			operator-=(size_t rhs)
			{
				address -= rhs;
				return *this;
			}
			
			modm_always_inline
			bool
			isValid() const
			{
				return (address != 0);
			}
			
			modm_always_inline
			const T*
			getPointer() const
			{
				return address;
			}
		
		private:
			const T* address;
		
		private:
			template <typename U>
			friend IOStream&
			operator << ( IOStream&, const Flash<U>&);
		};
		
		/**
		 * \brief	Convert a normal pointer to a accessor::Flash
		 * 
		 * \ingroup	accessor
		 */
		template<typename T>
		modm_always_inline ::modm::accessor::Flash<T>
		asFlash(const T* ptr)
		{
			return ::modm::accessor::Flash<T>(ptr);
		}
	}
}

// -----------------------------------------------------------------------------
// IMPLEMENTATION
// -----------------------------------------------------------------------------

/**
 * \brief	Streamoperator
 * 
 * \warning	Not implemented yet!!
 * \ingroup accessor
 */
template<typename T>
modm::IOStream&
operator << (modm::IOStream& os, modm::accessor::Flash<T> ptr)
{
	ptr.MODM_NOT_IMPLEMENTED_YET_streamoperator_of_not_char_type;
	// Not implemented YET
	// problem:
	//  Is ptr a pointer of an array?
	//  How many elements has the array?

	return os;
};

/**
 * \brief	Streamoperator - specialization for \c char
 * \ingroup accessor
 */
modm::IOStream&
operator << (modm::IOStream& os, modm::accessor::Flash<char> ptr);

#endif	// MODM_ACCESSOR_FLASH_HPP
