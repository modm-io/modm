/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2012, 2015-2016, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_SMART_POINTER_H
#define	MODM_SMART_POINTER_H

#include <cstring>		// for std::memcpy
#include <stdint.h>
#include <modm/architecture/utils.hpp>

#include <modm/io/iostream.hpp>

namespace modm
{
	class SmartPointerVolatile;

	/**
	 * \brief 	Container which destroys itself when the last
	 * 			copy is destroyed.
	 *
	 * This container saves a copy of the given data on the heap. It
	 * provides the functionality of a shared pointer => pointer object
	 * records when it is copied - when the last copy is destroyed the
	 * memory is released.
	 *
	 * \ingroup modm_container
	 */
	class SmartPointer
	{
	public:
		/// default constructor with empty payload
		SmartPointer();

		/**
		 * \brief	Allocates memory from the given size
		 *
		 * \param	size	the amount of memory to be allocated, has to be
		 * 					smaller than 65530
		 */
		SmartPointer(uint16_t size);

		// Must use a pointer to T here, otherwise the compiler can't distinguish
		// between constructor and copy constructor!
		template<typename T>
		explicit SmartPointer(const T *data)
		: ptr(new uint8_t[sizeof(T) + 4])
		{
			ptr[0] = 1;
			*reinterpret_cast<uint16_t*>(ptr + 2) = sizeof(T);
			std::memcpy(ptr + 4, data, sizeof(T));
		}

		SmartPointer(const SmartPointer& other);

		~SmartPointer();

		inline const uint8_t *
		getPointer() const
		{
			return ptr + 4;
		}

		inline uint8_t *
		getPointer()
		{
			return ptr + 4;
		}

		inline uint16_t
		getSize() const
		{
			return *reinterpret_cast<uint16_t*>(ptr + 2);
		}

	public:
		/**
		 * Get the value that are stored in the pointer casted to the given type.
		 * \note This method has no checking mechanism, use get(T) to have at least some.
		 *
		 * \return the stored value converted to \p T
		 */
		template<typename T>
		inline const T&
		get() const
		{
			return *reinterpret_cast<T*>(ptr + 4);
		}

		/**
		 * Get the value that are stored in the pointer casted to the given type.
		 * The method checks only the size but not the type of the stored data
		 *
		 * \return \c true if the type fit
		 */
		template<typename T>
		bool
		get(T& value) const
		{
			if (sizeof(T) == getSize())
			{
				value = *reinterpret_cast<T*>(ptr + 4);
				return true;
			}
			else {
				return false;
			}
		}

		bool
		operator == (const SmartPointer& other);

		SmartPointer&
		operator = (const SmartPointer& other);

	protected:
		uint8_t * ptr;

	protected:
		friend IOStream&
		operator <<( IOStream&, const SmartPointer&);
	} modm_packed;

	modm::IOStream&
	operator <<( modm::IOStream& s, const modm::SmartPointer& sPtr);

} // modm namespace

#endif	// MODM_SMART_POINTER_H
