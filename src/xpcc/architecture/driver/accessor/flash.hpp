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

#ifndef	XPCC_ACCESSOR__FLASH_HPP
#define	XPCC_ACCESSOR__FLASH_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/io/iostream.hpp>

#include "flash_reader.hpp"

namespace xpcc
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
			xpcc_always_inline
			explicit Flash(const T* addr = 0) :
				address(addr)
			{
			}
			
			template <typename U>
			xpcc_always_inline
			explicit Flash(const Flash<U>& rhs) :
				address((T*) rhs.address)
			{
			}
			
			xpcc_always_inline
			const T
			operator *() const
			{
				return FlashReader<T, sizeof(T)>::read(address);
			}
			
			xpcc_always_inline
			const T
			operator [](size_t index) const
			{
				return FlashReader<T, sizeof(T)>::read(address + index);
			}
			
			xpcc_always_inline
			Flash&
			operator++()
			{
				*this += 1;
				return *this;
			}

			xpcc_always_inline
			Flash
			operator++(int)
			{
				Flash ret = *this;
				++*this;
				return ret;
			}

			xpcc_always_inline
			Flash&
			operator--()
			{
				*this -= 1;
				return *this;
			}

			xpcc_always_inline
			Flash&
			operator--(int)
			{
				Flash ret = *this;
				--*this;
				return ret;
			}
			
			xpcc_always_inline
			Flash&
			operator+=(size_t rhs)
			{
				address += rhs;
				return *this;
			}
			
			xpcc_always_inline
			Flash&
			operator-=(size_t rhs)
			{
				address -= rhs;
				return *this;
			}
			
			xpcc_always_inline
			bool
			isValid() const
			{
				return (address != 0);
			}
			
			xpcc_always_inline
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
		xpcc_always_inline ::xpcc::accessor::Flash<T>
		asFlash(const T* ptr)
		{
			return ::xpcc::accessor::Flash<T>(ptr);
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
xpcc::IOStream&
operator << (xpcc::IOStream& os, xpcc::accessor::Flash<T> ptr)
{
	ptr.XPCC_NOT_IMPLEMENTED_YET_streamoperator_of_not_char_type;
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
xpcc::IOStream&
operator << (xpcc::IOStream& os, xpcc::accessor::Flash<char> ptr);

#endif	// XPCC_ACCESSOR__FLASH_HPP
