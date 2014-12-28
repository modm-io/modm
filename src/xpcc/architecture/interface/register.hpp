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
#include <xpcc/architecture/detect.hpp>

/**
 * @ingroup		interface
 * @defgroup	register	General Purpose Register classes
 *
 * These data structures are used to describe the relationship of single bits,
 * bit groups and bit configurations in registers with type-safe access.
 *
 * @author	Niklas Hauser
 */

namespace xpcc
{

/**
 * Register Base class
 *
 * @ingroup	register
 * @author	Niklas Hauser
 */
template< typename T >
struct Register
{
	typedef T UnderlyingType;
	T value;

	constexpr Register()
	:	value(0) {}


	/// @cond
	// The safe-bool idiom is not required here, since we are not
	// dealing with complex objects, but with one integral value.
	// So there is not need for it.
	/* explicit */
	constexpr operator bool() const
	{ return value != 0; }
	constexpr bool operator!() const
	{ return value == 0; }

	// GCC is broken here, matches overloads for deleted operators
#if not (defined XPCC__CPU_HOSTED and defined XPCC__COMPILER_GCC)
	// do NOT cast to anything else
	template<typename U>
	constexpr operator U() const = delete;
#endif
	/// @endcond

protected:
	constexpr Register(T value)
	:	value(value) {}
};
typedef Register<uint8_t> Register8;
typedef Register<uint16_t> Register16;
typedef Register<uint32_t> Register32;

/**
 * Flags class
 *
 * @ingroup	register
 * @author	Niklas Hauser
 */
template < typename Enum, typename T >
struct Flags : public ::xpcc::Register<T>
{
	typedef Enum Type;

	constexpr Flags() {}

	explicit constexpr Flags(T flag) :
		Register<T>(flag) {}
	explicit constexpr Flags(Register<T> const &o) :
		Register<T>(o.value) {}

	constexpr Flags(Enum flag) :
		Register<T>(T(flag)) {}
	constexpr Flags(Flags const &o) :
		Register<T>(o.value) {}

	// convenience methods
	// set
	inline void set(Enum const &flag)
	{ *this |= flag; }
	inline void set(Flags const &o)
	{ *this |= o; }

	// reset
	inline void reset(Enum const &flag)
	{ *this &= ~flag; }
	inline void reset(Flags const &o)
	{ *this &= ~o; }

	// update
	inline void update(Enum const &flag, bool value)
	{
		if (value) *this |= flag;
		else *this &= ~flag;
	}
	inline void update(Flags const &o, bool value)
	{
		if (value) *this |= o;
		else *this &= ~o;
	}

	// read single bits
	constexpr bool all(Enum const &flag) const
	{ return (*this & flag); }
	constexpr bool any(Enum const &flag) const
	{ return (*this & flag); }
	constexpr bool none(Enum const &flag) const
	{ return !(*this & flag); }

	// read multiple bits
	constexpr bool all(Flags const &o) const
	{ return (*this & o) == o; }
	constexpr bool any(Flags const &o) const
	{ return *this & o; }
	constexpr bool none(Flags const &o) const
	{ return (*this & o).value == 0; }

	static constexpr Flags mask()
	{ return Flags(T(-1)); }

	/// @cond
	// flags binary operations
	constexpr Flags operator~() const { return Flags(~Register<T>::value); }

	constexpr Flags operator&(Flags const &o) const { return Flags(Register<T>::value & o.value); }
	constexpr Flags operator|(Flags const &o) const { return Flags(Register<T>::value | o.value); }
	constexpr Flags operator^(Flags const &o) const { return Flags(Register<T>::value ^ o.value); }
	Flags & operator&=(Flags const &o) {  return (Register<T>::value &= o.value, *this); }
	Flags & operator|=(Flags const &o) { return (Register<T>::value |= o.value, *this); }
	Flags & operator^=(Flags const &o) { return (Register<T>::value ^= o.value, *this); }

	// enum binary operations
	constexpr Flags operator&(Enum const &flag) const { return operator&(Flags(flag)); }
	constexpr Flags operator|(Enum const &flag) const { return operator|(Flags(flag)); }
	constexpr Flags operator^(Enum const &flag) const { return operator^(Flags(flag)); }
	Flags & operator&=(Enum const &flag) { return operator&=(Flags(flag)); }
	Flags & operator|=(Enum const &flag) { return operator|=(Flags(flag)); }
	Flags & operator^=(Enum const &flag) { return operator^=(Flags(flag)); }

	friend constexpr Flags operator|(Enum const &a, Flags const &b) { return b | a; }
	friend constexpr Flags operator&(Enum const &a, Flags const &b) { return b & a; }
	friend constexpr Flags operator^(Enum const &a, Flags const &b) { return b ^ a; }

	// assignment
	Flags & operator=(Flags const &o) { return (Register<T>::value = o.value, *this); }

	// comparison
	constexpr bool operator==(Enum const &flag) const { return Register<T>::value == T(flag); }
	constexpr bool operator==(Flags const &o) const { return Register<T>::value == o.value; }

	constexpr bool operator!=(Enum const &flag) const { return Register<T>::value != T(flag); }
	constexpr bool operator!=(Flags const &o) const { return Register<T>::value != o.value; }

	friend constexpr bool operator==(Enum const &a, Flags const &b) { return b == a; }
	friend constexpr bool operator!=(Enum const &a, Flags const &b) { return b != a; }
	/// @endcond
};

template < typename Enum = uint8_t >
using Flags8 = Flags<Enum, uint8_t>;
template < typename Enum = uint16_t >
using Flags16 = Flags<Enum, uint16_t>;
template < typename Enum = uint32_t >
using Flags32 = Flags<Enum, uint32_t>;

/**
 * Value class
 *
 * @ingroup	register
 * @author	Niklas Hauser
 */
template < typename Parent, typename Parent::UnderlyingType Length, typename Parent::UnderlyingType Position >
struct Value : public ::xpcc::Register<typename Parent::UnderlyingType>
{
private:
	typedef typename Parent::UnderlyingType PType;
	static constexpr PType Mask = ((1 << Length) - 1) << Position;
public:
	explicit constexpr Value(PType config) :
		Register<PType>((config << Position) & Mask) {}
	constexpr Value(Value const &o) :
		Register<PType>(o.value) {}

	constexpr operator Parent() const
	{	return Parent(Register<PType>::value); }

	static constexpr PType get(Parent const &parent)
	{	return (parent.value & Mask) >> Position; }

	static inline void reset(Parent &parent)
	{	parent.value &= ~Mask; }

	static inline void set(Parent &parent, PType config)
	{	parent.value = (parent.value & ~Mask) | ((config << Position) & Mask); }

	static constexpr Parent mask()
	{ return Parent(Mask); }
};

/**
 * Configuration class
 *
 * @ingroup	register
 * @author	Niklas Hauser
 */
template < typename Parent, typename Enum, typename Parent::UnderlyingType Mask, typename Parent::UnderlyingType Position = 0 >
struct Configuration : public ::xpcc::Register<typename Parent::UnderlyingType>
{
private:
	typedef typename Parent::UnderlyingType PType;
	typedef typename Parent::Type PEnum;
public:
	explicit constexpr Configuration(PType config) :
		Register<PType>(config << Position) {}
	explicit constexpr Configuration(Enum config) :
		Register<PType>(PType(config) << Position) {}
	constexpr Configuration(Configuration const &o) :
		Register<PType>(o.value) {}

	constexpr operator Parent() const
	{	return Parent(Register<PType>::value); }

	static constexpr Enum get(Parent const &parent)
	{	return Enum((parent.value & (Mask << Position)) >> Position); }

	static inline void reset(Parent &parent)
	{	parent.value &= ~(Mask << Position); }

	static inline void set(Parent &parent, Enum config)
	{	parent.value = (parent.value & ~(Mask << Position)) | (PType(config) << Position); }

	static constexpr Parent mask()
	{ return Parent((Mask << Position)); }
};

}	// namespace xpcc

// these operator overloadings will overload *ALL* possible enum classes
/*
template <typename Enum>
constexpr ::xpcc::Flags<Enum> operator compl (Enum const &lhs) { return compl ::xpcc::Flags<Enum>(lhs); }
template <typename Enum>
constexpr ::xpcc::Flags<Enum> operator|(Enum const &a, Enum const &b) { return ::xpcc::Flags<Enum>(a) | b;  }
template <typename Enum>
constexpr ::xpcc::Flags<Enum> operator&(Enum const &a, Enum const &b) { return ::xpcc::Flags<Enum>(a) & b; }
template <typename Enum>
constexpr ::xpcc::Flags<Enum> operator^(Enum const &a, Enum const &b) { return ::xpcc::Flags<Enum>(a) ^ b; }
//*/

/// @hideinitializer
#define XPCC_TYPE_FLAGS(name) \
	XPCC_INTERNAL_FLAGS(name, friend)

/// @hideinitializer
#define XPCC_EXT_TYPE_FLAGS(name) \
	XPCC_INTERNAL_FLAGS(name,)

/// @hideinitializer
#define XPCC_FLAGS8(name) \
	typedef ::xpcc::Flags8<name> CONCAT(name, _t); \
	XPCC_TYPE_FLAGS(CONCAT(name, _t))

/// @hideinitializer
#define XPCC_FLAGS16(name) \
	typedef ::xpcc::Flags16<name> CONCAT(name, _t); \
	XPCC_TYPE_FLAGS(CONCAT(name, _t))

/// @hideinitializer
#define XPCC_FLAGS32(name) \
	typedef ::xpcc::Flags32<name> CONCAT(name, _t); \
	XPCC_TYPE_FLAGS(CONCAT(name, _t))

/// @hideinitializer
#define XPCC_FLAGS_GROUP(name, first, ...) \
	struct name : public ::xpcc::Register<first::UnderlyingType> { \
	constexpr name() {} \
	explicit constexpr name(UnderlyingType value) \
	:	Register(value) {} \
	XPCCFG_GET_MACRO(first, __VA_ARGS__, \
			XPCCFG_30,XPCCFG_29,XPCCFG_28,XPCCFG_27,XPCCFG_26, \
			XPCCFG_25,XPCCFG_24,XPCCFG_23,XPCCFG_22,XPCCFG_21, \
			XPCCFG_20,XPCCFG_19,XPCCFG_18,XPCCFG_17,XPCCFG_16, \
			XPCCFG_15,XPCCFG_14,XPCCFG_13,XPCCFG_12,XPCCFG_11, \
			XPCCFG_10,XPCCFG_9, XPCCFG_8, XPCCFG_7, XPCCFG_6,  \
			XPCCFG_5, XPCCFG_4, XPCCFG_3, XPCCFG_2, XPCCFG_1)(name, first, __VA_ARGS__) \
	};

/// @cond
#define XPCC_INTERNAL_FLAGS(name, scope) \
	scope constexpr name operator compl (name::Type const &lhs) \
	{ return compl name(lhs); } \
	XPCC_INTERNAL_FLAGS_EOP(type, name, &, scope) \
	XPCC_INTERNAL_FLAGS_EOP(type, name, |, scope) \
	XPCC_INTERNAL_FLAGS_EOP(type, name, ^, scope)

#define XPCC_INTERNAL_FLAGS_EOP(type, name, op, scope) \
	scope constexpr name operator op (name::Type const &lhs, name::Type const &rhs) \
	{ return name(name::Type(name::UnderlyingType(lhs) op name::UnderlyingType(rhs))); }

#define XPCC_FG(name, reg) \
	constexpr name(reg::Type value) \
	:	Register(UnderlyingType(value)) {} \
	constexpr name(reg value) \
	:	Register(value.value) {}

#define XPCCFG_1(name, reg     ) XPCC_FG(name, reg)
#define XPCCFG_2(name, reg, ...) XPCC_FG(name, reg)XPCCFG_1(name, __VA_ARGS__)
#define XPCCFG_3(name, reg, ...) XPCC_FG(name, reg)XPCCFG_2(name, __VA_ARGS__)
#define XPCCFG_4(name, reg, ...) XPCC_FG(name, reg)XPCCFG_3(name, __VA_ARGS__)
#define XPCCFG_5(name, reg, ...) XPCC_FG(name, reg)XPCCFG_4(name, __VA_ARGS__)
#define XPCCFG_6(name, reg, ...) XPCC_FG(name, reg)XPCCFG_5(name, __VA_ARGS__)
#define XPCCFG_7(name, reg, ...) XPCC_FG(name, reg)XPCCFG_6(name, __VA_ARGS__)
#define XPCCFG_8(name, reg, ...) XPCC_FG(name, reg)XPCCFG_7(name, __VA_ARGS__)
#define XPCCFG_9(name, reg, ...) XPCC_FG(name, reg)XPCCFG_8(name, __VA_ARGS__)
#define XPCCFG_10(name, reg, ...) XPCC_FG(name, reg)XPCCFG_9(name, __VA_ARGS__)
#define XPCCFG_11(name, reg, ...) XPCC_FG(name, reg)XPCCFG_10(name, __VA_ARGS__)
#define XPCCFG_12(name, reg, ...) XPCC_FG(name, reg)XPCCFG_11(name, __VA_ARGS__)
#define XPCCFG_13(name, reg, ...) XPCC_FG(name, reg)XPCCFG_12(name, __VA_ARGS__)
#define XPCCFG_14(name, reg, ...) XPCC_FG(name, reg)XPCCFG_13(name, __VA_ARGS__)
#define XPCCFG_15(name, reg, ...) XPCC_FG(name, reg)XPCCFG_14(name, __VA_ARGS__)
#define XPCCFG_16(name, reg, ...) XPCC_FG(name, reg)XPCCFG_15(name, __VA_ARGS__)
#define XPCCFG_17(name, reg, ...) XPCC_FG(name, reg)XPCCFG_16(name, __VA_ARGS__)
#define XPCCFG_18(name, reg, ...) XPCC_FG(name, reg)XPCCFG_17(name, __VA_ARGS__)
#define XPCCFG_19(name, reg, ...) XPCC_FG(name, reg)XPCCFG_18(name, __VA_ARGS__)
#define XPCCFG_20(name, reg, ...) XPCC_FG(name, reg)XPCCFG_19(name, __VA_ARGS__)
#define XPCCFG_21(name, reg, ...) XPCC_FG(name, reg)XPCCFG_20(name, __VA_ARGS__)
#define XPCCFG_22(name, reg, ...) XPCC_FG(name, reg)XPCCFG_21(name, __VA_ARGS__)
#define XPCCFG_23(name, reg, ...) XPCC_FG(name, reg)XPCCFG_22(name, __VA_ARGS__)
#define XPCCFG_24(name, reg, ...) XPCC_FG(name, reg)XPCCFG_23(name, __VA_ARGS__)
#define XPCCFG_25(name, reg, ...) XPCC_FG(name, reg)XPCCFG_24(name, __VA_ARGS__)
#define XPCCFG_26(name, reg, ...) XPCC_FG(name, reg)XPCCFG_25(name, __VA_ARGS__)
#define XPCCFG_27(name, reg, ...) XPCC_FG(name, reg)XPCCFG_26(name, __VA_ARGS__)
#define XPCCFG_28(name, reg, ...) XPCC_FG(name, reg)XPCCFG_27(name, __VA_ARGS__)
#define XPCCFG_29(name, reg, ...) XPCC_FG(name, reg)XPCCFG_28(name, __VA_ARGS__)
#define XPCCFG_30(name, reg, ...) XPCC_FG(name, reg)XPCCFG_29(name, __VA_ARGS__)
#define XPCCFG_31(name, reg, ...) XPCC_FG(name, reg)XPCCFG_30(name, __VA_ARGS__)

#define XPCCFG_GET_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,\
		_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,...) _31
/// @endcond


#endif // XPCC_REGISTER_HPP
