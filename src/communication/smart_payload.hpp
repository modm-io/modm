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
 * $Id: backend_interface.h 32 2009-09-17 17:37:50Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_SMART_PAYLOAD_H
#define	XPCC_SMART_PAYLOAD_H

#include <string.h>		// for memcpy
#include <stdint.h>

class SmartPayload
{
public:
	// Must use a pointer to T here, otherwise the compiler can't distinguish
	// between constructor and copy constructor!
	template<typename T>
	SmartPayload(const T *data) 
	: ptr(new uint8_t[sizeof(T) + 2])
	{
		ptr[0] = 1;
		ptr[1] = sizeof(T);
		memcpy(ptr + 2, data, sizeof(T));
	}
	
	SmartPayload(const SmartPayload& other) 
	: ptr(other.ptr)
	{
		ptr[0]++;
	}
	
	~SmartPayload() {
		if (--ptr[0] == 0) {
			delete ptr;
		}
	}
	
	const uint8_t *
	getPointer() const {
		return &ptr[2];
	}
	
	uint8_t
	getSize() const {
		return ptr[1];
	}
	
private:
	SmartPayload&
	operator=(const SmartPayload& other);
	
	uint8_t * const ptr;
};

#endif	// XPCC_SMART_PAYLOAD_H
