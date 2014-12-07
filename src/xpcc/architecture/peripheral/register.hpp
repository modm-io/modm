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

static constexpr uint8_t Bit0   = 0b00000001;
static constexpr uint8_t Bit1   = 0b00000010;
static constexpr uint8_t Bit2   = 0b00000100;
static constexpr uint8_t Bit3   = 0b00001000;
static constexpr uint8_t Bit4   = 0b00010000;
static constexpr uint8_t Bit5   = 0b00100000;
static constexpr uint8_t Bit6   = 0b01000000;
static constexpr uint8_t Bit7   = 0b10000000;

static constexpr uint16_t Bit8  = 0b0000000100000000;
static constexpr uint16_t Bit9  = 0b0000001000000000;
static constexpr uint16_t Bit10 = 0b0000010000000000;
static constexpr uint16_t Bit11 = 0b0000100000000000;
static constexpr uint16_t Bit12 = 0b0001000000000000;
static constexpr uint16_t Bit13 = 0b0010000000000000;
static constexpr uint16_t Bit14 = 0b0100000000000000;
static constexpr uint16_t Bit15 = 0b1000000000000000;

static constexpr uint32_t Bit16 = 0b000000010000000000000000;
static constexpr uint32_t Bit17 = 0b000000100000000000000000;
static constexpr uint32_t Bit18 = 0b000001000000000000000000;
static constexpr uint32_t Bit19 = 0b000010000000000000000000;
static constexpr uint32_t Bit20 = 0b000100000000000000000000;
static constexpr uint32_t Bit21 = 0b001000000000000000000000;
static constexpr uint32_t Bit22 = 0b010000000000000000000000;
static constexpr uint32_t Bit23 = 0b100000000000000000000000;

static constexpr uint32_t Bit24 = 0b00000001000000000000000000000000;
static constexpr uint32_t Bit25 = 0b00000010000000000000000000000000;
static constexpr uint32_t Bit26 = 0b00000100000000000000000000000000;
static constexpr uint32_t Bit27 = 0b00001000000000000000000000000000;
static constexpr uint32_t Bit28 = 0b00010000000000000000000000000000;
static constexpr uint32_t Bit29 = 0b00100000000000000000000000000000;
static constexpr uint32_t Bit30 = 0b01000000000000000000000000000000;
static constexpr uint32_t Bit31 = 0b10000000000000000000000000000000;

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
