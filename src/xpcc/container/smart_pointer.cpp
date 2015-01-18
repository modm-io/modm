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
 */
// ----------------------------------------------------------------------------

#include "smart_pointer.hpp"

// ----------------------------------------------------------------------------
// must allocate at least three bytes, so getPointer() does return
// a valid address
xpcc::SmartPointer::SmartPointer() :
	ptr(new uint8_t[3])
{
	ptr[0] = 1;
	ptr[1] = 0;
}

xpcc::SmartPointer::SmartPointer(const SmartPointer& other) :
	ptr(other.ptr)
{
	retain();
}

// must allocate at least three bytes, so getPointer() does return
// a valid address
xpcc::SmartPointer::SmartPointer(uint8_t size) :
	ptr(new uint8_t[size + 3])
{
	ptr[0] = 1;
	ptr[1] = size;
}

xpcc::SmartPointer::~SmartPointer()
{
	release();
}

// ----------------------------------------------------------------------------
bool
xpcc::SmartPointer::operator == (const SmartPointer& other)
{
	return (this->ptr == other.ptr);
}

// ----------------------------------------------------------------------------
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& s, const xpcc::SmartPointer& v)
{
	s << "0x" << xpcc::hex;
	for (uint8_t i = 2; i < v.ptr[1] + 2; i++) {
		s << v.ptr[i];
	}
	s << xpcc::ascii;
	return s;
}
