// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_SMART_POINTER_H
#define	XPCC_SMART_POINTER_H

#include <string.h>		// for memcpy
#include <stdint.h>

#include <xpcc/io/iostream.hpp>

namespace xpcc
{
	class SmartPointerVolatile;

	/**
	 * \brief 	Container which destroyes itself when the last
	 * 			copy is destroyed.
	 * 
	 * This container saves a copy of the given data on the heap. It
	 * provides the functionality of a shared pointer => pointer object
	 * records when it is copied - when the last copy is destroyed the
	 * memory is released.
	 * 
	 * \ingroup container
	 */
	class SmartPointer
	{
	public:
		// default constructor with empty payload
		SmartPointer();
		
		// Must use a pointer to T here, otherwise the compiler can't distinguish
		// between constructor and copy constructor!
		template<typename T>
		explicit SmartPointer(const T *data)
		: ptr(new uint8_t[sizeof(T) + 2])
		{
			ptr[0] = 1;
			ptr[1] = sizeof(T);
			memcpy(ptr + 2, data, sizeof(T));
		}

		SmartPointer(const SmartPointer& other);
		
		SmartPointer(const SmartPointerVolatile& other);

		~SmartPointer();

		inline const uint8_t *
		getPointer() const
		{
			return &ptr[2];
		}
		
		inline uint8_t
		getSize() const {
			return ptr[1];
		}
		
		/**
		 * Get the value that are stored in the pointer casted to the given type.
		 * \note This method has no checking mechanism, use get(T) to have at least some.
		 *
		 * \return the stored value converted to T
		 */
		template<typename T>
		const T&
		get() const
		{
			return *(T*)&ptr[2];
		}

		/**
		 * Get the value that are stored in the pointer casted to the given type.
		 * The method checks only the size but not the typ of the stored data
		 * 
		 * \return true if the type fit
		 */
		template<typename T>
		bool
		get(T& value) const
		{
			if( sizeof(T) == ptr[1] ) {
				value = *(T*)&ptr[2];
				return true;
			}
			else {
				return false;
			}
		}

	protected:
		friend IOStream&
		operator <<( IOStream&, const SmartPointer&);

		/**
		 * \brief	Allocates memory from the given size
		 * 
		 * \param	size	the amount of memory to be allocated, has to be
		 * 					smaller than 252
		 */
		SmartPointer(uint8_t size);
		
		SmartPointer&
		operator=(const SmartPointer& other);
		
		uint8_t * const ptr;
	};

	/**
	 * \ingroup container
	 */
	xpcc::IOStream&
	operator <<( xpcc::IOStream& s, const xpcc::SmartPointer& sPtr);

	/**
	 * \brief 	With this class a \b SmartPointer can be created, off that the
	 * 			memory is writable.
	 *
	 * Use the \b SmartPointerVolatile to allocate writable memory and then
	 * typecast with the copyconstructor to \b SmartPointer to get an object with
	 * write protected data.
	 * 
	 * \ingroup container
	 */
	class SmartPointerVolatile : public SmartPointer
	{
		public:
			/**
			 * \brief	Allocates memory from the given size
			 * 
			 * \param	size	the amount of memory to be allocated, has to
			 * 					be smaller than 252
			 */
			SmartPointerVolatile(uint8_t size);
			
			SmartPointerVolatile(const SmartPointerVolatile& other);

			inline uint8_t *
			getPointer()
			{
				return &ptr[2];
			}

		protected :
			friend IOStream&
			operator <<( IOStream&, const SmartPointerVolatile&);
	};

	/**
	 * \ingroup container
	 */
	xpcc::IOStream&
	operator <<( xpcc::IOStream& s, const xpcc::SmartPointerVolatile& sPtr);
};

#endif	// XPCC_SMART_POINTER_H
