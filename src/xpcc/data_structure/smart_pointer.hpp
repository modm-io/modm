// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * $Id: smart_payload.hpp 89 2009-10-17 15:50:23Z thundernail $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_SMART_POINTER_H
#define	XPCC_SMART_POINTER_H

#include <string.h>		// for memcpy
#include <stdint.h>

namespace xpcc
{
	class SmartPointer
	{
	public:
		// default constructor with empty payload
		SmartPointer();

		// Must use a pointer to T here, otherwise the compiler can't distinguish
		// between constructor and copy constructor!
		template<typename T>
		SmartPointer(const T *data)
		: ptr(new uint8_t[sizeof(T) + 2])
		{
			ptr[0] = 1;
			ptr[1] = sizeof(T);
			memcpy(ptr + 2, data, sizeof(T));
		}

		SmartPointer(const SmartPointer& other);
		
		/// Allocates memory from the given size
		/// @param size the amount of memory to be allocated, has to be smaller than 253
		SmartPointer(uint8_t size);

		~SmartPointer();
		
		uint8_t *
		getPointer();

		const uint8_t *
		getPointer() const;
		
		inline uint8_t
		getSize() const {
			return ptr[1];
		}
		
	private:
		SmartPointer&
		operator=(const SmartPointer& other);
		
		uint8_t * const ptr;
	};
}

#endif	// XPCC_SMART_POINTER_H
