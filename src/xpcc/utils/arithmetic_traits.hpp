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

#ifndef XPCC__ARITHMETIC_TRAITS_HPP
#define XPCC__ARITHMETIC_TRAITS_HPP

#include <stdint.h>
#include <xpcc/utils/macros.hpp>

namespace xpcc
{
	/**
	 * \ingroup		utils
	 * \defgroup	arithmetic_trais	Arithmetic Traits
	 * \brief		Traits to give numbers more information then they have by
	 * 				default in C++
	 *
	 * \section provides	Values provided by these traits
	 *
	 * \li \c	DoubleType	Type that can holds the doubled length of data.
	 * 						May be used to hold the result of a multiplication.
	 * \li \c	SignedType	Signed type for the given type. It applies 
	 * 						<i>T == SignedType</i> if T is already signed.
	 * \li \c	UnsignedType	Some as SignedType only for unsigned types
	 * \li \c	min()		smallest value.
	 * \li \c	max()		biggest value
	 * \li \c	isSigned	is this a signed or unsigned type
	 * \li \c	decimalDigits	count of digits to display this type in decimal
	 * 
	 * \section usage	Usage
	 * \code
	 * typedef typename xpcc::ArithmeticTraits<T>::DoubleType T_DOUBLE;
	 *
	 * T min = xpcc::ArithmeticTraits<T>::min();
	 * T max = xpcc::ArithmeticTraits<T>::max();
	 * \endcode
	 * 
	 * \version	$Id$
	 * \author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
	 */
	// ------------------------------------------------------------------------
	/*\{*/
	template<typename T>
	struct ArithmeticTraits
	{
	};
	
	// ------------------------------------------------------------------------

	template<>
	struct ArithmeticTraits<char>
	{
		typedef int16_t DoubleType;
		typedef signed char SignedType;
		typedef unsigned char UnsignedType;
		
		static const unsigned char decimalDigits = 4; // inc sign
	};

	// ------------------------------------------------------------------------

	template<>
	struct ArithmeticTraits<signed char>
	{
		typedef int16_t DoubleType;
		typedef int8_t SignedType;
		typedef uint8_t UnsignedType;

		static const unsigned char decimalDigits = 4; // inc sign
		static const bool isSigned = true;
		
		static ALWAYS_INLINE signed char
		min()
		{
			return -127 - 1;
		}
		
		static ALWAYS_INLINE signed char
		max()
		{
			return 127;
		}
	};

	// ------------------------------------------------------------------------
	template<>
	struct ArithmeticTraits<unsigned char>
	{
		typedef uint16_t DoubleType;
		typedef int8_t SignedType;
		typedef uint8_t UnsignedType;

		static const uint8_t decimalDigits = 3;
		static const bool isSigned = false;
		
		static ALWAYS_INLINE unsigned char
		min()
		{
			return 0;
		}
		
		static ALWAYS_INLINE unsigned char
		max()
		{
			return 255;
		}
	};

	// ------------------------------------------------------------------------
	template<>
	struct ArithmeticTraits<int16_t>
	{
		typedef int32_t DoubleType;
		typedef int16_t SignedType;
		typedef uint16_t UnsignedType;

		static const uint8_t decimalDigits = 6; // inc. sign
		static const bool isSigned = true;
		
		static ALWAYS_INLINE int16_t
		min()
		{
			return -32767 - 1;
		}
		
		static ALWAYS_INLINE int16_t
		max()
		{
			return 32767;
		}
	};

	// ------------------------------------------------------------------------
	template<>
	struct ArithmeticTraits<uint16_t>
	{
		typedef uint32_t DoubleType;
		typedef int16_t SignedType;
		typedef uint16_t UnsignedType;
		
		static const uint8_t decimalDigits = 6;
		static const bool isSigned = false;
		
		static ALWAYS_INLINE uint16_t
		min()
		{
			return 0;
		}
		
		static ALWAYS_INLINE uint16_t
		max()
		{
			return 65535;
		}
	};
	
	// ------------------------------------------------------------------------
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

		static const uint8_t decimalDigits = 11; // inc. sign
		static const bool isSigned = true;
		
		static ALWAYS_INLINE int32_t
		min()
		{
			return -2147483647L - 1;
		}
		
		static ALWAYS_INLINE int32_t
		max()
		{
			return 2147483647L;
		}
	};
	
	// ------------------------------------------------------------------------
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

		static const uint8_t decimalDigits = 10;
		static const bool isSigned = false;
		
		static ALWAYS_INLINE uint32_t
		min()
		{
			return 0;
		}
		
		static ALWAYS_INLINE uint32_t
		max()
		{
			return 4294967295UL;
		}
	};
	
	// ------------------------------------------------------------------------
	template<>
	struct ArithmeticTraits<int64_t>
	{
		typedef double DoubleType;
		typedef int64_t SignedType;
		typedef uint64_t UnsignedType;

		static const uint8_t decimalDigits = 20; // inc. sign
		static const bool isSigned = true;
		
		static ALWAYS_INLINE int64_t
		min()
		{
			return -9223372036854775807LL - 1;
		}
		
		static ALWAYS_INLINE uint64_t
		max()
		{
			return 9223372036854775807LL;
		}
	};
	
	// ------------------------------------------------------------------------
	template<>
	struct ArithmeticTraits<uint64_t>
	{
		typedef double DoubleType;
		typedef int64_t SignedType;
		typedef uint64_t UnsignedType;

		static const uint8_t decimalDigits = 20;
		static const bool isSigned = false;
		
		static ALWAYS_INLINE uint64_t
		min()
		{
			return 0;
		}
		
		static ALWAYS_INLINE uint64_t
		max()
		{
			return 18446744073709551615ULL;
		}
	};
	
	// ------------------------------------------------------------------------

	template<>
	struct ArithmeticTraits<float>
	{
		typedef float DoubleType;
		typedef float SignedType;
		typedef float UnsignedType;
		
		static const bool isSigned = true;
		
		static ALWAYS_INLINE float
		min()
		{
			return -3.40282e+38;
		}
		
		static ALWAYS_INLINE float
		max()
		{
			return 3.40282e+38;
		}
	};

	// ------------------------------------------------------------------------

#ifdef __AVR__
	// the AVRs don't know a dedicated double type, double is implemented
	// with sizeof(double) == sizeof(float).
	template<>
	struct ArithmeticTraits<double>
	{
		typedef double DoubleType;
		typedef double SignedType;
		typedef double UnsignedType;
		
		static const bool isSigned = true;
		
		static ALWAYS_INLINE double
		min()
		{
			return -3.40282e+38;
		}
		
		static ALWAYS_INLINE double
		max()
		{
			return 3.40282e+38;
		}
	};

#else

	template<>
	struct ArithmeticTraits<double>
	{
		typedef double DoubleType;
		typedef double SignedType;
		typedef double UnsignedType;
		
		static const bool isSigned = true;
		
		static ALWAYS_INLINE double
		min()
		{
			return 2.22507e-308;
		}
		
		static ALWAYS_INLINE double
		max()
		{
			return 1.79769e+308;
		}
	};

#endif
	/*\}*/
}

#endif	// XPCC__ARITHMETIC_TRAITS_HPP
