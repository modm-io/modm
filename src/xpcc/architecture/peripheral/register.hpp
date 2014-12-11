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
#include <xpcc/utils/bit_constants.hpp>

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
struct Register
{
	typedef T Type;
	T value{0};

	constexpr Register()
	:	value(0) {}

	constexpr operator bool() const
	{ return value != 0; }

	// do NOT cast to anything else
	template<typename U>
	operator U() const = delete;

protected:
	constexpr Register(T value)
	:	value(value) {}
};
typedef Register<uint8_t> Register8;
typedef Register<uint16_t> Register16;
typedef Register<uint32_t> Register32;

}	// namespace xpcc

/// @hideinitializer
#define REGISTER8(name) \
	INTERNAL_REGISTER(uint8_t, name, friend)

/// @hideinitializer
#define REGISTER16(name) \
	INTERNAL_REGISTER(uint16_t, name, friend)

/// @hideinitializer
#define REGISTER32(name) \
	INTERNAL_REGISTER(uint32_t, name, friend)

/// @hideinitializer
#define REGISTER8_GROUP(name, ...) \
	INTERNAL_REGISTER_GROUP(uint8_t, name, __VA_ARGS__)

/// @hideinitializer
#define REGISTER16_GROUP(name, ...) \
	INTERNAL_REGISTER_GROUP(uint16_t, name, __VA_ARGS__)

/// @hideinitializer
#define REGISTER32_GROUP(name, ...) \
	INTERNAL_REGISTER_GROUP(uint32_t, name, __VA_ARGS__)

/// @cond
#define INTERNAL_REGISTER(type, name, scope) \
	struct CONCAT(name, _t) : public ::xpcc::Register<type> { \
		constexpr CONCAT(name, _t)() {} \
		explicit constexpr CONCAT(name, _t)(type value) \
		:	Register(value) {} \
		constexpr CONCAT(name, _t)(name value) \
		:	Register(static_cast<type>(value)) {} \
		INTERNAL_REGISTER_OP(type, name, &) \
		INTERNAL_REGISTER_OP(type, name, |) \
		INTERNAL_REGISTER_OP(type, name, ^) \
	}; \
	scope constexpr CONCAT(name, _t) operator compl (CONCAT(name, _t) const &lhs) \
	{ return CONCAT(name, _t)(static_cast<name>(compl lhs.value)); } \
	scope constexpr CONCAT(name, _t) operator compl (name const &lhs) \
	{ return CONCAT(name, _t)(static_cast<name>(compl static_cast<type>(lhs))); } \
	INTERNAL_REGISTER_EOP(type, name, &, scope) \
	INTERNAL_REGISTER_EOP(type, name, |, scope) \
	INTERNAL_REGISTER_EOP(type, name, ^, scope)

#define INTERNAL_REGISTER_EOP(type, name, op, scope) \
	scope constexpr CONCAT(name, _t) operator op (name const &lhs, CONCAT(name, _t) const &rhs) \
	{ return CONCAT(name, _t)(static_cast<name>(static_cast<type>(lhs) op rhs.value)); } \
	scope constexpr CONCAT(name, _t) operator op (CONCAT(name, _t) const &lhs, name const &rhs) \
	{ return CONCAT(name, _t)(static_cast<name>(static_cast<type>(rhs) op lhs.value)); } \
	scope constexpr CONCAT(name, _t) operator op (CONCAT(name, _t) const &lhs, CONCAT(name, _t) const &rhs) \
	{ return CONCAT(name, _t)(static_cast<name>(lhs.value op rhs.value)); } \
	scope constexpr CONCAT(name, _t) operator op (name const &lhs, name const &rhs) \
	{ return CONCAT(name, _t)(static_cast<name>(static_cast<type>(lhs) op static_cast<type>(rhs))); }

#define INTERNAL_REGISTER_OP(type, name, op) \
	inline CONCAT(name, _t)& operator CONCAT(op, =) (CONCAT(name, _t) const &rhs) \
	{ value CONCAT(op, =) rhs.value; return *this; } \
	inline CONCAT(name, _t)& operator CONCAT(op, =) (name const &rhs) \
	{ value CONCAT(op, =) static_cast<type>(rhs); return *this; }

#define INTERNAL_REGISTER_GROUP(type, name, ...) \
	struct CONCAT(name, _t) : public ::xpcc::Register<type> { \
	constexpr CONCAT(name, _t)() {} \
	explicit constexpr CONCAT(name, _t)(type value) \
	:	Register(value) {} \
	INT_RG_GET_MACRO(__VA_ARGS__, \
			INT_RG_30,INT_RG_29,INT_RG_28,INT_RG_27,INT_RG_26, \
			INT_RG_25,INT_RG_24,INT_RG_23,INT_RG_22,INT_RG_21, \
			INT_RG_20,INT_RG_19,INT_RG_18,INT_RG_17,INT_RG_16, \
			INT_RG_15,INT_RG_14,INT_RG_13,INT_RG_12,INT_RG_11, \
			INT_RG_10,INT_RG_9,INT_RG_8,INT_RG_7,INT_RG_6, \
			INT_RG_5,INT_RG_4,INT_RG_3,INT_RG_2,INT_RG_1)(name, __VA_ARGS__) \
	};

#define INTERNAL_RG(name, reg) \
	constexpr CONCAT(name, _t)(reg value) \
	:	Register(static_cast<Type>(value)) {} \
	constexpr CONCAT(name, _t)(CONCAT(reg, _t) value) \
	:	Register(value.value) {}

#define INT_RG_1(name, reg     ) INTERNAL_RG(name, reg)
#define INT_RG_2(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_1(name, __VA_ARGS__)
#define INT_RG_3(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_2(name, __VA_ARGS__)
#define INT_RG_4(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_3(name, __VA_ARGS__)
#define INT_RG_5(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_4(name, __VA_ARGS__)
#define INT_RG_6(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_5(name, __VA_ARGS__)
#define INT_RG_7(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_6(name, __VA_ARGS__)
#define INT_RG_8(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_7(name, __VA_ARGS__)
#define INT_RG_9(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_8(name, __VA_ARGS__)
#define INT_RG_10(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_9(name, __VA_ARGS__)
#define INT_RG_11(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_10(name, __VA_ARGS__)
#define INT_RG_12(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_11(name, __VA_ARGS__)
#define INT_RG_13(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_12(name, __VA_ARGS__)
#define INT_RG_14(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_13(name, __VA_ARGS__)
#define INT_RG_15(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_14(name, __VA_ARGS__)
#define INT_RG_16(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_15(name, __VA_ARGS__)
#define INT_RG_17(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_16(name, __VA_ARGS__)
#define INT_RG_18(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_17(name, __VA_ARGS__)
#define INT_RG_19(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_18(name, __VA_ARGS__)
#define INT_RG_20(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_19(name, __VA_ARGS__)
#define INT_RG_21(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_20(name, __VA_ARGS__)
#define INT_RG_22(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_21(name, __VA_ARGS__)
#define INT_RG_23(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_22(name, __VA_ARGS__)
#define INT_RG_24(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_23(name, __VA_ARGS__)
#define INT_RG_25(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_24(name, __VA_ARGS__)
#define INT_RG_26(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_25(name, __VA_ARGS__)
#define INT_RG_27(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_26(name, __VA_ARGS__)
#define INT_RG_28(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_27(name, __VA_ARGS__)
#define INT_RG_29(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_28(name, __VA_ARGS__)
#define INT_RG_30(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_29(name, __VA_ARGS__)
#define INT_RG_31(name, reg, ...) INTERNAL_RG(name, reg)INT_RG_30(name, __VA_ARGS__)

#define INT_RG_GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,\
		_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,...) _31
/// @endcond


#endif // XPCC_REGISTER_HPP
