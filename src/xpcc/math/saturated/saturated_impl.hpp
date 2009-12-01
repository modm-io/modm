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
 * $Id: saturated_impl.hpp 94 2009-10-19 18:44:57Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__SATURATED_HPP
	#error	"Don't include this file directly use 'math/saturated.hpp' instead!"
#endif

#ifdef __AVR__
	// include faster implementations written in assembler
	#include "saturated__avr_u8_impl.hpp"
	#include "saturated__avr_s8_impl.hpp"
#endif

namespace xpcc
{
	template<typename T>
	Saturated<T>::Saturated() : 
		value()
	{
	}
	
	template<typename T>
	Saturated<T>::Saturated(const T& initialValue) : 
		value(initialValue)
	{
	}
	
	template<typename T>
	void
	Saturated<T>::setValue(DoubleType in)
	{
		if (in > ArithmeticTraits<T>::maxValue) {
			value = ArithmeticTraits<T>::maxValue;
		}
		else if (in < ArithmeticTraits<T>::minValue) {
			value = ArithmeticTraits<T>::minValue;
		}
		else {
			value = static_cast<T>(in);
		}
	}
	
	template<typename T>
	Saturated<T>&
	Saturated<T>::operator += (const Saturated& other)
	{
		DoubleType temp = static_cast<DoubleType>(this->value) +
						  static_cast<DoubleType>(other.value);
		setValue(temp);
		
		return *this;
	}
	
	template<typename T>
	Saturated<T>&
	Saturated<T>::operator -= (const Saturated& other)
	{
		DoubleType temp = static_cast<DoubleType>(value) -
						  static_cast<DoubleType>(other.value);
		setValue(temp);
		
		return *this;
	}
	
	template<typename T>
	void
	Saturated<T>::absolute()
	{
		if (ArithmeticTraits<T>::isSigned)
		{
			if (value < 0) {
				value = -value;
			}
		}
	}
	
	// ----------------------------------------------------------------------------
	template<typename U>
	Saturated<U>
	operator - (const Saturated<U>& x)
	{
		typedef typename ArithmeticTraits<U>::DoubleType DoubleType;
		
		DoubleType temp = - static_cast<DoubleType>(x.value);
		
		Saturated<U> result;
		result.setValue(temp);
		return result;
	}

	// ----------------------------------------------------------------------------
	template<typename U>
	Saturated<U>
	abs(const Saturated<U>& x)
	{
		Saturated<U> result(x);
		
		if (ArithmeticTraits<U>::isSigned) {
			result.absolute();
		}
		return result;
	}

	// ----------------------------------------------------------------------------
	template<typename U>
	Saturated<U>
	operator - (const Saturated<U>& a, const Saturated<U>& b)
	{
		Saturated<U> t(a);
		t -= b;
		return t;
	}
	
	// ----------------------------------------------------------------------------
	template<typename U>
	Saturated<U>
	operator + (const Saturated<U>& a, const Saturated<U>& b)
	{
		Saturated<U> t(a);
		t += b;
		return t;
	}
	
	// ----------------------------------------------------------------------------
	template<typename U>
	inline bool
	operator == (const Saturated<U>& a, const Saturated<U>& b)
	{
		return (a.value == b.value);
	}

	// ----------------------------------------------------------------------------
	template<typename U>
	inline bool
	operator != (const Saturated<U>& a, const Saturated<U>& b)
	{
		return (a.value != b.value);
	}
	
	// ----------------------------------------------------------------------------
	
	template<>
	void
	Saturated<uint8_t>::absolute()
	{
	}
	
	template<>
	void
	Saturated<uint16_t>::absolute()
	{
	}
	
	template<>
	void
	Saturated<uint32_t>::absolute()
	{
	}
}
