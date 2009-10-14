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

#ifndef	XPCC__SATURATED_HPP
#define	XPCC__SATURATED_HPP

#include "../utils/arithmetic_traits.hpp"

namespace xpcc
{
	/// \ingroup	math
	/// \brief	Saturated arithmetics
	/// 
	/// \see http://www.mikrocontroller.net/articles/AVR_Arithmetik/Saturierung
	///
	/// \author	Fabian Greif <fabian.greif@rwth-aachen.de>
	/// 
	/// \todo	extend implementation (multiplication etc.)
	/// \todo	add 16-bit datetype assembler implementations for AVRs
	/// \todo	documentation
	template<typename T>
	class Saturated
	{
		typedef typename ArithmeticTraits<T>::DoubleType DoubleType;
	public:
		typedef typename ArithmeticTraits<T>::SignedType SignedType;
		typedef typename ArithmeticTraits<T>::UnsignedType UnsignedType;
		
		friend class Saturated<SignedType>;
		friend class Saturated<UnsignedType>;
	
	public:
		Saturated(const T& initalValue=0) : 
			value(initalValue) {
		}
		
		const T&
		getValue() const {
			return value;
		}
		
		Saturated&
		operator+=(const Saturated<SignedType>& other);
		
		Saturated&
		operator+=(const Saturated<UnsignedType>& other);
		
		Saturated&
		operator-=(const Saturated<SignedType>& other);
		
		Saturated&
		operator-=(const Saturated<UnsignedType>& other);
	
	public:
		template<typename U>
		friend Saturated<U>
		operator-(const Saturated<U>& x);
		
		template<typename U>
		friend Saturated<U>
		abs(const Saturated<U>& x);
		
		template<typename U>
		friend Saturated<U>
		operator-(const Saturated<U>& a, const Saturated<U>& b);

		template<typename U>
		friend Saturated<U>
		operator+(const Saturated<U>& a, const Saturated<U>& b);
		
		template<typename U>
		friend bool
		operator==(const Saturated<U>& a, const Saturated<U>& b);
		
		template<typename U>
		friend bool
		operator!=(const Saturated<U>& a, const Saturated<U>& b);
	
	private:
		static T
		limitValue(DoubleType value);
		
		T value;
	};
	
	// ------------------------------------------------------------------------
	
	/// \brief	Invert value
	template<typename T>
	Saturated<typename Saturated<T>::SignedType>
	operator-(const Saturated<T>& x);
	
	/// \brief	Calculate the absolte value
	template<typename T>
	Saturated<T>
	abs(const Saturated<T>& x);

	template<typename T>
	Saturated<T>
	operator-(const Saturated<T>& a, const Saturated<T>& b);

	template<typename T>
	Saturated<T>
	operator+(const Saturated<T>& a, const Saturated<T>& b);
	
	template<typename T>
	bool
	operator==(const Saturated<T>& a, const Saturated<T>& b);
	
	template<typename T>
	bool
	operator!=(const Saturated<T>& a, const Saturated<T>& b);
}

#include "saturated/saturated_impl.hpp"

#endif	// XPCC__SATURATED_HPP
