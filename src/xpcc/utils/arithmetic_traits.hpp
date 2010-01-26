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
 * $Id: arithmetic_traits.hpp 83 2009-10-15 19:58:57Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__ARITHMETIC_TRAITS_HPP
#define XPCC__ARITHMETIC_TRAITS_HPP

#include <stdint.h>

namespace xpcc
{
	/**
	 * @ingroup		utils
	 * @defgroup	arithmetic_trais	Arithmetic Traits
	 * @brief		Traits to give numbers more information then they have by
	 * 				default in C++
	 *
	 * @section provides	Values provided by these traits
	 *
	 * @li @c	DoubleType	Type that can holds the doubled length of data.
	 * 						May be used to hold the result of a multiplication.
	 * @li @c	SignedType	Signed type for the given type. It applies 
	 * 						<i>T == SignedType</i> if T is already signed.
	 * @li @c	UnsignedType	Some as SignedType only for unsigned types
	 * @li @c	minValue	smallest value.
	 * @li @c	maxValue	biggest value
	 * @li @c	isSigned	is this a signed or unsigned type
	 * @li @c	isInteger	is this an integer type
	 * @li @c	isFloat		is this a floatingpoint type (float, double)
	 * @li @c	decimalDigits	count of digits to display this type in decimal
	 * 
	 * @section usage	Usage
	 * @code
	 * typedef typename xpcc::ArithmeticTraits<T>::DoubleType T_DOUBLE;
	 *
	 * T min = xpcc::ArithmeticTraits<T>::minValue;
	 * T max = xpcc::ArithmeticTraits<T>::maxValue;
	 * @endcode
	 * 
	 * @version	$Id: arithmetic_traits.hpp 83 2009-10-15 19:58:57Z dergraaf $
	 * @author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
	 */
	// -------------------------------------------------------------------------
	/*@{*/
	template<typename T>
	struct ArithmeticTraits
	{
	};

	// -------------------------------------------------------------------------

	template<>
	struct ArithmeticTraits<char>
	{
		typedef int16_t DoubleType;
		typedef signed char SignedType;
		typedef unsigned char UnsignedType;

		static const char minValue = -0x80;
		static const char maxValue =  0x7F;
		static const unsigned char decimalDigits = 4; // inc sign
	};

	// -------------------------------------------------------------------------

	template<>
	struct ArithmeticTraits<signed char>
	{
		typedef int16_t DoubleType;
		typedef signed char SignedType;
		typedef unsigned char UnsignedType;

		static const char minValue = -0x80;
		static const char maxValue =  0x7F;
		static const unsigned char decimalDigits = 4; // inc sign

		static const bool isInteger = true;
		static const bool isFloat = false;
		static const bool isSigned = true;
	};

	// -------------------------------------------------------------------------
	template<>
	struct ArithmeticTraits<unsigned char>
	{
		typedef uint16_t DoubleType;
		typedef signed char SignedType;
		typedef unsigned char UnsignedType;

		static const uint8_t minValue = 0;
		static const uint8_t maxValue = 0xff;
		static const uint8_t decimalDigits = 3;

		static const bool isInteger = true;
		static const bool isFloat = false;
		static const bool isSigned = false;
	};

	// -------------------------------------------------------------------------
	template<>
	struct ArithmeticTraits<int16_t>
	{
		typedef int32_t DoubleType;
		typedef int16_t SignedType;
		typedef uint16_t UnsignedType;

		static const int16_t minValue = -0x8000;
		static const int16_t maxValue =  0x7FFF;
		static const uint8_t decimalDigits = 6; // inc. sign

		static const bool isInteger = true;
		static const bool isFloat = false;
		static const bool isSigned = true;
	};

	// -------------------------------------------------------------------------
	template<>
	struct ArithmeticTraits<uint16_t>
	{
		typedef uint32_t DoubleType;
		typedef int16_t SignedType;
		typedef uint16_t UnsignedType;

		static const uint16_t minValue = 0;
		static const uint16_t maxValue = 0xFFFF;
		static const uint8_t decimalDigits = 6;
	
		static const bool isInteger = true;
		static const bool isFloat = false;
		static const bool isSigned = false;
	};
	
	// -------------------------------------------------------------------------
	template<>
	struct ArithmeticTraits<int32_t>
	{
#ifdef __AVR__
		typedef float DoubleType; // int64_t is on AVRs only a int32_t
#else
		typedef int64_t DoubleType;
#endif
		typedef int32_t SignedType;
		typedef uint32_t UnsignedType;

		static const int32_t minValue = -0x80000000;
		static const int32_t maxValue =  0x7FFFFFFF;
		static const uint8_t decimalDigits = 11; // inc. sign
	
		static const bool isInteger = true;
		static const bool isFloat = false;
		static const bool isSigned = true;
	};
	
	// -------------------------------------------------------------------------
	template<>
	struct ArithmeticTraits<uint32_t>
	{
#ifdef __AVR__
		typedef float DoubleType; // int64_t is on AVRs only a int32_t
#else
		typedef uint64_t DoubleType;
#endif
		typedef int32_t SignedType;
		typedef uint32_t UnsignedType;

		static const uint32_t minValue = 0;
		static const uint32_t maxValue = 0xFFFFFFFF;
		static const uint8_t decimalDigits = 10;

		static const bool isInteger = true;
		static const bool isFloat = false;
		static const bool isSigned = false;
	};

	// -------------------------------------------------------------------------

	template<>
	struct ArithmeticTraits<float>
	{
		typedef float DoubleType;
		typedef float SignedType;
		typedef float UnsignedType;

		static const float minValue = -3.40282e+38;
		static const float maxValue = 3.40282e+38;
	
		static const bool isInteger = false;
		static const bool isFloat = true;
		static const bool isSigned = true;
	};

	// -------------------------------------------------------------------------

#ifdef __AVR__
	// the AVRs don't know a dedicated double type, double is implemented
	// with sizeof(double) == sizeof(float).
	template<>
	struct ArithmeticTraits<double>
	{
		typedef double DoubleType;
		typedef double SignedType;
		typedef double UnsignedType;

		static const double minValue = -3.40282e+38;
		static const double maxValue = 3.40282e+38;
	
		static const bool isInteger = false;
		static const bool isFloat = true;
		static const bool isSigned = true;
	};

#else

	template<>
	struct ArithmeticTraits<double>
	{
		typedef double DoubleType;
		typedef double SignedType;
		typedef double UnsignedType;

		static const double minValue = 2.22507e-308;
		static const double maxValue = 1.79769e+308;

		static const bool isInteger = false;
		static const bool isFloat = true;
		static const bool isSigned = true;
	};

#endif
	/*@}*/
}

#endif	// XPCC__ARITHMETIC_TRAITS_HPP
