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

#ifndef XPCC__IOSTREAM_HPP
	#error	"Don't include this file directly, use 'io/iostream.hpp' instead!"
#endif

#include <stdio.h>		// snprintf()
#include <stdlib.h>

#include <xpcc/math/utils.hpp>
#include <xpcc/utils/arithmetic_traits.hpp>
#include <xpcc/utils/typet.hpp>

// ----------------------------------------------------------------------------

template<typename T>
void
xpcc::IOStream::putInteger( T value )
{
	char str[ArithmeticTraits<T>::decimalDigits + 1]; // +1 for '\0'

	snprintf(str, sizeof(str), "%d", value);

	this->device->put(str);
}


// ----------------------------------------------------------------------------

template<typename T>
void
xpcc::IOStream::putFloat( const T& value )
{
	// hard coded for 2.22507e-308
	char str[13 + 1]; // +1 for '\0'
	
#ifdef __AVR__
	dtostre(value, str, 5, 0);
#else
	snprintf(str, sizeof(str), "%.5e", value);
#endif
	
	this->device->put(str);
}
