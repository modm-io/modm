// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ARITHMETIC_TRAITS_HPP
#define XPCC_ARITHMETIC_TRAITS_HPP

#include <stdint.h>

#include <xpcc/architecture/utils.hpp>
#include <xpcc/utils/template_metaprogramming.hpp>

namespace xpcc
{

/**
 * @ingroup		utils
 * @defgroup	arithmetic_trais	Arithmetic Traits
 *
 * Traits to give numbers more information then they have by
 * default in C++
 *
 * @section provides	Values provided by these traits
 *
 * - `WideType`			Type that can holds the doubled length of data.
 * 						May be used to hold the result of a multiplication.
 * - `SignedType`		Signed type for the given type. It applies
 * 						`T == SignedType` if `T` is already signed.
 * - `UnsignedType		Some as SignedType only for unsigned types
 * - `min`				smallest value.
 * - `max`				biggest value
 * - `isSigned`			is this a signed or unsigned type
 * - `isFloatingPoint`	is this a floating point type
 * - `isInteger`		is this a integer type
 * - `decimalDigits`	count of digits to display this type in decimal
 *
 * @section usage	Usage
 * @code
 * typedef typename xpcc::ArithmeticTraits<T>::WideType T_DOUBLE;
 *
 * T min = xpcc::ArithmeticTraits<T>::min();
 * T max = xpcc::ArithmeticTraits<T>::max();
 * @endcode
 *
 * @author	Martin Rosekeit <martin.rosekeit@rwth-aachen.de>
 * @author	Fabian Greif <fabian.greif@rwth-aachen.de>
 * @author	Niklas Hauser
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
	typedef int16_t WideType;
	typedef signed char SignedType;
	typedef unsigned char UnsignedType;

	static const unsigned char decimalDigits = 4; // inc sign
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;
};

// ------------------------------------------------------------------------

template<>
struct ArithmeticTraits<signed char>
{
	typedef int16_t WideType;
	typedef int8_t SignedType;
	typedef uint8_t UnsignedType;

	static const unsigned char decimalDigits = 4; // inc sign
	static const bool isSigned = true;
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;

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
	typedef uint16_t WideType;
	typedef int8_t SignedType;
	typedef uint8_t UnsignedType;

	static const uint8_t decimalDigits = 3;
	static const bool isSigned = false;
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;

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
	typedef int32_t WideType;
	typedef int16_t SignedType;
	typedef uint16_t UnsignedType;

	static const uint8_t decimalDigits = 6; // inc. sign
	static const bool isSigned = true;
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;

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
	typedef uint32_t WideType;
	typedef int16_t SignedType;
	typedef uint16_t UnsignedType;

	static const uint8_t decimalDigits = 5;
	static const bool isSigned = false;
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;

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
#if defined(XPCC__CPU_AVR)
	typedef float WideType; // int64_t is on AVRs only a int32_t
#else
	typedef int64_t WideType;
#endif
	typedef int32_t SignedType;
	typedef uint32_t UnsignedType;

	static const uint8_t decimalDigits = 11; // inc. sign
	static const bool isSigned = true;
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;

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
#if defined(XPCC__CPU_AVR)
	typedef float WideType; // int64_t is on AVRs only a int32_t
#else
	typedef uint64_t WideType;
#endif
	typedef int32_t SignedType;
	typedef uint32_t UnsignedType;

	static const uint8_t decimalDigits = 10;
	static const bool isSigned = false;
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;

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

#if defined(XPCC__CPU_ARM)
// ------------------------------------------------------------------------
// For ARM 'int32_t' is of type 'long'. Therefore there is no
// class here for the default type 'int'.
template<>
struct ArithmeticTraits<int>
{
	typedef int64_t WideType;
	typedef int32_t SignedType;
	typedef uint32_t UnsignedType;

	static const uint8_t decimalDigits = 11; // inc. sign
	static const bool isSigned = true;
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;

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

template<>
struct ArithmeticTraits<unsigned int>
{
	typedef uint64_t WideType;
	typedef int32_t SignedType;
	typedef uint32_t UnsignedType;

	static const uint8_t decimalDigits = 10;
	static const bool isSigned = false;
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;

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
#endif

// ------------------------------------------------------------------------
template<>
struct ArithmeticTraits<int64_t>
{
	typedef double WideType;
	typedef int64_t SignedType;
	typedef uint64_t UnsignedType;

	static const uint8_t decimalDigits = 20; // inc. sign
	static const bool isSigned = true;
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;

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
	typedef double WideType;
	typedef int64_t SignedType;
	typedef uint64_t UnsignedType;

	static const uint8_t decimalDigits = 20;
	static const bool isSigned = false;
	static const bool isFloatingPoint = false;
	static const bool isInteger = true;

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
	typedef float WideType;
	typedef float SignedType;
	typedef float UnsignedType;

	static const bool isSigned = true;
	static const bool isFloatingPoint = true;
	static const bool isInteger = false;

	static ALWAYS_INLINE float
	min()
	{
		return __FLT_MIN__;
	}

	static ALWAYS_INLINE float
	max()
	{
		return __FLT_MAX__;
	}

	static ALWAYS_INLINE float
	epsilon()
	{
		return __FLT_EPSILON__;
	}
};

// ------------------------------------------------------------------------
template<>
struct ArithmeticTraits<double>
{
	typedef double WideType;
	typedef double SignedType;
	typedef double UnsignedType;

	static const bool isSigned = true;
	static const bool isFloatingPoint = true;
	static const bool isInteger = false;

	static ALWAYS_INLINE double
	min()
	{
		return __DBL_MIN__;
	}

	static ALWAYS_INLINE double
	max()
	{
		return __DBL_MAX__;
	}

	static ALWAYS_INLINE double
	epsilon()
	{
		return __DBL_EPSILON__;
	}
};

//// @}
} // namespace xpcc

#endif	// XPCC_ARITHMETIC_TRAITS_HPP
