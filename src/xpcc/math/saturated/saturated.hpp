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
 * $Id: saturated.hpp 94 2009-10-19 18:44:57Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__SATURATED_HPP
#define	XPCC__SATURATED_HPP

#include <xpcc/utils/arithmetic_traits.hpp>

namespace xpcc
{
	/**
	 * \ingroup	math
	 * \brief	Saturated arithmetics
	 *  
	 * \see http://www.mikrocontroller.net/articles/AVR_Arithmetik/Saturierung
	 * 
	 * \author	Fabian Greif <fabian.greif@rwth-aachen.de>
	 *  
	 * \todo	extend implementation (multiplication etc.)
	 * \todo	add 16-bit datetype assembler implementations for AVRs
	 * \todo	documentation
	 */
	template<typename T>
	class Saturated
	{
		typedef typename ArithmeticTraits<T>::SignedType SignedType;
		typedef typename ArithmeticTraits<SignedType>::DoubleType DoubleType;
		
	public:
		Saturated();
		
		Saturated(const T& initialValue);
		
		inline const T&
		getValue() const
		{
			return value;
		}
		
		Saturated&
		operator += (const Saturated& other);
		
		Saturated&
		operator -= (const Saturated& other);
		
		void
		absolute();
	
	public:
		template<typename U>
		friend Saturated<U>
		operator - (const Saturated<U>& x);
		
		template<typename U>
		friend Saturated<U>
		abs(const Saturated<U>& x);
		
		template<typename U>
		friend Saturated<U>
		operator - (const Saturated<U>& a, const Saturated<U>& b);

		template<typename U>
		friend Saturated<U>
		operator + (const Saturated<U>& a, const Saturated<U>& b);
		
		template<typename U>
		friend bool
		operator == (const Saturated<U>& a, const Saturated<U>& b);
		
		template<typename U>
		friend bool
		operator != (const Saturated<U>& a, const Saturated<U>& b);
		
		// TODO > >= < <=
	
	private:
		void
		setValue(DoubleType value);
		
		T value;
	};
	
	// ------------------------------------------------------------------------
	
	/// \brief	Invert value
	template<typename U>
	Saturated<U>
	operator - (const Saturated<U>& x);
	
	/// \brief	Calculate the absolute value
	template<typename U>
	Saturated<U>
	abs(const Saturated<U>& x);

	template<typename U>
	Saturated<U>
	operator - (const Saturated<U>& a, const Saturated<U>& b);

	template<typename U>
	Saturated<U>
	operator + (const Saturated<U>& a, const Saturated<U>& b);
	
	template<typename U>
	inline bool
	operator == (const Saturated<U>& a, const Saturated<U>& b);
	
	template<typename U>
	inline bool
	operator != (const Saturated<U>& a, const Saturated<U>& b);
}

#include "saturated_impl.hpp"

#endif	// XPCC__SATURATED_HPP
