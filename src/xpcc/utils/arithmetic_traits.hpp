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
 * T min = xpcc::ArithmeticTraits<T>::min;
 * T max = xpcc::ArithmeticTraits<T>::max;
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

	static constexpr unsigned char decimalDigits = 4; // inc sign
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;
};

// ------------------------------------------------------------------------

template<>
struct ArithmeticTraits<signed char>
{
	typedef int16_t WideType;
	typedef int8_t SignedType;
	typedef uint8_t UnsignedType;

	static constexpr unsigned char decimalDigits = 4; // inc sign
	static constexpr bool isSigned = true;
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;

	static constexpr signed char min = -127 - 1;
	static constexpr signed char max = 127;
};

// ------------------------------------------------------------------------
template<>
struct ArithmeticTraits<unsigned char>
{
	typedef uint16_t WideType;
	typedef int8_t SignedType;
	typedef uint8_t UnsignedType;

	static constexpr uint8_t decimalDigits = 3;
	static constexpr bool isSigned = false;
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;

	static constexpr unsigned char min = 0;
	static constexpr unsigned char max = 255;
};

// ------------------------------------------------------------------------
template<>
struct ArithmeticTraits<int16_t>
{
	typedef int32_t WideType;
	typedef int16_t SignedType;
	typedef uint16_t UnsignedType;

	static constexpr uint8_t decimalDigits = 6; // inc. sign
	static constexpr bool isSigned = true;
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;

	static constexpr int16_t min = -32767 - 1;
	static constexpr int16_t max = 32767;
};

// ------------------------------------------------------------------------
template<>
struct ArithmeticTraits<uint16_t>
{
	typedef uint32_t WideType;
	typedef int16_t SignedType;
	typedef uint16_t UnsignedType;

	static constexpr uint8_t decimalDigits = 5;
	static constexpr bool isSigned = false;
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;

	static constexpr uint16_t min = 0;
	static constexpr uint16_t max = 65535;
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

	static constexpr uint8_t decimalDigits = 11; // inc. sign
	static constexpr bool isSigned = true;
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;

	static constexpr int32_t min = -2147483647L - 1;
	static constexpr int32_t max = 2147483647L;
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

	static constexpr uint8_t decimalDigits = 10;
	static constexpr bool isSigned = false;
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;

	static constexpr uint32_t min = 0;
	static constexpr uint32_t max = 4294967295UL;
};

#if defined(XPCC__CPU_ARM) && defined(XPCC__OS_NONE)
// ------------------------------------------------------------------------
// For ARM 'int32_t' is of type 'long'. Therefore there is no
// class here for the default type 'int'.
template<>
struct ArithmeticTraits<int>
{
	typedef int64_t WideType;
	typedef int32_t SignedType;
	typedef uint32_t UnsignedType;

	static constexpr uint8_t decimalDigits = 11; // inc. sign
	static constexpr bool isSigned = true;
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;

	static constexpr int32_t min = -2147483647L - 1;
	static constexpr int32_t max = 2147483647L;
};

template<>
struct ArithmeticTraits<unsigned int>
{
	typedef uint64_t WideType;
	typedef int32_t SignedType;
	typedef uint32_t UnsignedType;

	static constexpr uint8_t decimalDigits = 10;
	static constexpr bool isSigned = false;
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;

	static constexpr uint32_t min = 0;
	static constexpr uint32_t max = 4294967295UL;
};
#endif

// ------------------------------------------------------------------------
template<>
struct ArithmeticTraits<int64_t>
{
	typedef double WideType;
	typedef int64_t SignedType;
	typedef uint64_t UnsignedType;

	static constexpr uint8_t decimalDigits = 20; // inc. sign
	static constexpr bool isSigned = true;
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;

	static constexpr int64_t min = -9223372036854775807LL - 1;
	static constexpr uint64_t max = 9223372036854775807LL;
};

// ------------------------------------------------------------------------
template<>
struct ArithmeticTraits<uint64_t>
{
	typedef double WideType;
	typedef int64_t SignedType;
	typedef uint64_t UnsignedType;

	static constexpr uint8_t decimalDigits = 20;
	static constexpr bool isSigned = false;
	static constexpr bool isFloatingPoint = false;
	static constexpr bool isInteger = true;

	static constexpr uint64_t min = 0;
	static constexpr uint64_t max = 18446744073709551615ULL;
};

// ------------------------------------------------------------------------
template<>
struct ArithmeticTraits<float>
{
	typedef float WideType;
	typedef float SignedType;
	typedef float UnsignedType;

	static constexpr bool isSigned = true;
	static constexpr bool isFloatingPoint = true;
	static constexpr bool isInteger = false;

	static constexpr float min = __FLT_MIN__;
	static constexpr float max = __FLT_MAX__;
	static constexpr float epsilon = __FLT_EPSILON__;
};

// ------------------------------------------------------------------------
template<>
struct ArithmeticTraits<double>
{
	typedef double WideType;
	typedef double SignedType;
	typedef double UnsignedType;

	static constexpr bool isSigned = true;
	static constexpr bool isFloatingPoint = true;
	static constexpr bool isInteger = false;

	static constexpr double min = __DBL_MIN__;
	static constexpr double max = __DBL_MAX__;
	static constexpr double epsilon = __DBL_EPSILON__;
};

//// @}
} // namespace xpcc

#endif	// XPCC_ARITHMETIC_TRAITS_HPP
