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
struct RegisterBit
{
	typedef T Type;
	T value{0};

	constexpr RegisterBit(const T bit)
	:	value(bit) {}
};


template< typename T >
struct RegisterBitGroup
{
	typedef T Type;
	T value{0};

protected:
	constexpr RegisterBitGroup(const T value)
	:	value(value) {}
};


template< typename T >
struct RegisterBitGroupConfig
{
	typedef T Type;
	T value{0};

	constexpr RegisterBitGroupConfig(const T value)
	:	value(value) {}
};


template< typename T >
struct Register
{
	typedef T Type;
	T value{0};

	constexpr Register(const Register<T> &value)
	:	value(value.value) {}

	constexpr Register(Register<T> &value)
	:	value(value.value) {}

	operator bool() const
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


#define REGISTER8(name, ...) \
	struct name { \
	uint8_t value; \
	constexpr STest(uint8_t value) \
	:	value(value) {} \
	constexpr operator uint8_t() const \
	{ return value; } \
	enum : uint8_t { \
		__VA_ARGS__ \
	}; };

#define REGISTER_GROUP(name, type) \
struct name : public type { \
	using type::type; \
};

#define REGISTER(name, type, values) \
	struct name : public type { \
		using type::type; \
		friend constexpr bool \
		operator==(const name& lhs, const name& rhs) \
		{ return (lhs.value == rhs.value); } \
		friend constexpr bool \
		operator!=(const name& lhs, const name& rhs) \
		{ return !(lhs == rhs); } \
		template<typename O> bool \
		operator==(const O&) const = delete; \
		\
		friend constexpr name operator | (name const &lhs, name const &rhs) \
		{ return name(name::Bit(lhs.value | rhs.value)); } \
		inline name& operator |= (name const &rhs) \
		{ *this = *this | rhs; return *this; } \
		friend constexpr name operator & (name const &lhs, name const &rhs) \
		{ return name(name::Bit(lhs.value & rhs.value)); } \
		inline name& operator &= (name const &rhs) \
		{ *this = *this & rhs; return *this; } \
		friend constexpr name operator ^ (name const &lhs, name const &rhs) \
		{ return name(name::Bit(lhs.value ^ rhs.value)); } \
		inline name& operator ^= (name const &rhs) \
		{ *this = *this ^ rhs; return *this; } \
		friend constexpr name operator ~ (name const &rhs) \
		{ return name(name::Bit(~rhs.value)); } \
		\
		struct Bit : public xpcc::RegisterBit<Type> { \
			using RegisterBit::RegisterBit; \
			operator type() const \
			{ return type(name(Bit(value))); } \
		}; \
		constexpr name(const Bit &bit) \
		:	type(bit.value) {} \
		\
		struct Group : public xpcc::RegisterBitGroup<Type> { \
			using RegisterBitGroup::RegisterBitGroup; \
			operator type() const \
			{ return type(name(Group(value))); } \
			\
			struct Config : public xpcc::RegisterBitGroupConfig<Type> { \
				using RegisterBitGroupConfig::RegisterBitGroupConfig; \
				operator type() const \
				{ return type(name(Config(value))); } \
			}; \
			constexpr Group(const Config &config) \
			:	Group(config.value) {} \
			friend constexpr Group operator | (Config const &lhs, Config const &rhs) \
			{ return Group(lhs.value | rhs.value); } \
			friend constexpr Group operator & (Config const &lhs, Config const &rhs) \
			{ return Group(lhs.value & rhs.value); } \
			friend constexpr Group operator ^ (Config const &lhs, Config const &rhs) \
			{ return Group(lhs.value ^ rhs.value); } \
			friend constexpr Group operator ~ (Config const &rhs) \
			{ return Group(~rhs.value); } \
		}; \
		constexpr name(const Group &group) \
		:	type(group.value) {} \
		constexpr name(const Group::Config &config) \
		:	type(config.value) {} \
		\
		friend constexpr bool \
		operator==(const name& lhs, const Bit& rhs) \
		{ return (lhs.value == rhs.value); } \
		friend constexpr bool \
		operator==(const name& lhs, const Group& rhs) \
		{ return (lhs.value == rhs.value); } \
		friend constexpr bool \
		operator==(const name& lhs, const Group::Config& rhs) \
		{ return (lhs.value == rhs.value); } \
		\
		values; \
	};

#define REGISTER_BIT(value) \
	static constexpr Bit value;

#define REGISTER_CONFIG_GROUP(name, width, values,...) \
	struct name : public Group { \
		using Group::Group; \
		private: \
		static constexpr Type Position = 0; \
		public: \
		static constexpr Config Mask = ((1 << width) - 1) << Position; \
			values; \
	}; \
	INT_RBG_GET_MACRO(__VA_ARGS__,INT_RBG_10,INT_RBG_9,INT_RBG_8,INT_RBG_7,INT_RBG_6, \
			INT_RBG_5,INT_RBG_4,INT_RBG_3,INT_RBG_2,INT_RBG_1)(((1 << width) - 1), values, __VA_ARGS__)

#define INTERNAL_REGISTER_BITGROUP(mask, values, name, position) \
	struct name : public Group { \
		using Group::Group; \
		static constexpr Type Position = position; \
		static constexpr Config Mask = (mask) << Position; \
		values; \
	};

#define REGISTER_CONFIG_INSTANCE(name, position) \
	name, position

#define REGISTER_CONFIG(value) \
	static constexpr Config value << Position;


#define INT_RBG_1(mask, values, name, position     )
#define INT_RBG_2(mask, values, name, position, ...) INTERNAL_REGISTER_BITGROUP(mask, values, name, position)
#define INT_RBG_3(mask, values, name, position, ...)
#define INT_RBG_4(mask, values, name, position, ...) INTERNAL_REGISTER_BITGROUP(mask, values, name, position)INT_RBG_2(mask, values, __VA_ARGS__)
#define INT_RBG_5(mask, values, name, position, ...)
#define INT_RBG_6(mask, values, name, position, ...) INTERNAL_REGISTER_BITGROUP(mask, values, name, position)INT_RBG_4(mask, values, __VA_ARGS__)
#define INT_RBG_7(mask, values, name, position, ...)
#define INT_RBG_8(mask, values, name, position, ...) INTERNAL_REGISTER_BITGROUP(mask, values, name, position)INT_RBG_6(mask, values, __VA_ARGS__)
#define INT_RBG_9(mask, values, name, position, ...)
#define INT_RBG_10(mask, values, name, position, ...) INTERNAL_REGISTER_BITGROUP(mask, values, name, position)INT_RBG_8(mask, values, __VA_ARGS__)

#define INT_RBG_GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,NAME,...) NAME


#endif // XPCC_REGISTER_HPP
