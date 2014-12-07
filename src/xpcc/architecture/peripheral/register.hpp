// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_REGISTER_HPP
#define XPCC_REGISTER_HPP

#include <stdint.h>

/**
 * @ingroup		peripheral
 * @defgroup	register	General Purpose Register classes
 *
 * These data structures are used to describe the relationship of single bits,
 * bit groups and bit configurations in registers with type-safe access.
 *
 * @author	Niklas Hauser
 */

namespace xpcc
{

template< typename T >
struct register_t
{
	typedef T Type;
	T value{0};

	constexpr register_t()
	:	value(0) {}

	constexpr register_t(T value)
	:	value(value) {}

	constexpr operator T() const
	{ return value; }
};

typedef ::xpcc::register_t<uint8_t> register8_t;
typedef ::xpcc::register_t<uint16_t> register16_t;
typedef ::xpcc::register_t<uint32_t> register32_t;

}	// namespace xpcc

/// @cond
#define INT_REGISTER(type, name, ...) \
	struct name : public ::xpcc::register_t<type> { \
		using register_t::register_t; \
		enum : type { \
			__VA_ARGS__ }; \
	};
/// @endcond

/// @hideinitializer
#define REGISTER8(name, ...) \
	INT_REGISTER(uint8_t, name, __VA_ARGS__)

/// @hideinitializer
#define REGISTER16(name, ...) \
	INT_REGISTER(uint16_t, name, __VA_ARGS__)

/// @hideinitializer
#define REGISTER32(name, ...) \
	INT_REGISTER(uint32_t, name, __VA_ARGS__)



#endif // XPCC_REGISTER_HPP
