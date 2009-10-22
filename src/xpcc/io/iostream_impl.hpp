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

#include <stdio.h>

#include <xpcc/utils/arithmetic_traits.hpp>
#include <xpcc/utils/typet.hpp>

template<typename T>
inline xpcc::IOStream&
xpcc::IOStream::operator<< ( const T& v )
{
	// typedef (T.is_integer) ? IntegerWriter<T> : ObjectWriter<T>
	typedef typename xpcc::tm::Select <
			::xpcc::ArithmeticTraits<T>::isFloat,
			::xpcc::FloatWriter<T>,
			::xpcc::StringWriter >::Result NotIntegerWriter;

    typedef typename xpcc::tm::Select <
			::xpcc::ArithmeticTraits<T>::isInteger,
			::xpcc::IntegerWriter<T>,
			NotIntegerWriter >::Result Writer;

    Writer()(*this, v);

	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
xpcc::IOStream&
xpcc::IOStream::putInteger( T value )
{
	char str[ArithmeticTraits<T>::digits10 + 1]; // +1 for '\0'

	// TODO use a optimized function to format output
	snprintf(str, sizeof(str), "%d", value);

	this->device->put(str);

	return *this;
}

// ----------------------------------------------------------------------------

template<typename T>
xpcc::IOStream&
xpcc::IOStream::putFloat( T value )
{
	// TODO is hard coded for 2.22507e-308
	char str[13 + 1]; // +1 for '\0'

	// TODO use a optimized function to format output
	snprintf(str, sizeof(str), "%e", value);

	this->device->put(str);

	return *this;
}

// ----------------------------------------------------------------------------

inline xpcc::IOStream&
xpcc::endl(IOStream& ios)
{
	return flush(ios.put('\n'));
}

// ----------------------------------------------------------------------------

inline xpcc::IOStream&
xpcc::flush(IOStream& ios)
{
	return ios.flush();
}
