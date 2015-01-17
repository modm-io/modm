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
#if not (defined XPCC__OS_HOSTED and defined XPCC__COMPILER_GCC)
	// do NOT cast to anything else
	template<typename U>
	constexpr operator U() const = delete;
#endif
	/// @endcond

protected:
	constexpr Register(T value)
	:	value(value) {}
};
/// @ingroup	register
/// @{
typedef Register<uint8_t> Register8;
typedef Register<uint16_t> Register16;
typedef Register<uint32_t> Register32;
/// @}

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

/// @ingroup	register
/// @{
template < typename Enum = uint8_t >
using Flags8 = Flags<Enum, uint8_t>;
template < typename Enum = uint16_t >
using Flags16 = Flags<Enum, uint16_t>;
template < typename Enum = uint32_t >
using Flags32 = Flags<Enum, uint32_t>;
/// @}

/**
 * Flags Group class.
 *
 * @ingroup	register
 * @author	Niklas Hauser
 */
template < typename... T >
struct FlagsGroup {};

/// @cond
template < typename T, typename... Ts >
struct FlagsGroup<T, Ts...> : public FlagsGroup<Ts...>
{
	// use inherited constructors
	using FlagsGroup<Ts...>::FlagsGroup;

	// empty constructor
	constexpr FlagsGroup() {};
	// enum class
	constexpr FlagsGroup(typename T::Type value)
	:	FlagsGroup<Ts...>(typename T::UnderlyingType(value)) {}
	// Flags class
	constexpr FlagsGroup(T value)
	:	FlagsGroup<Ts...>(value.value) {}
};

template < typename T >
struct FlagsGroup<T> : public Register<typename T::UnderlyingType>
{
	// empty constructor
	constexpr FlagsGroup() {};
	// copy constructor
	constexpr FlagsGroup(FlagsGroup const &o)
	: 	Register<typename T::UnderlyingType>(o.value) {}
	// explicit underlying type constructor
	explicit constexpr FlagsGroup(typename T::UnderlyingType value)
	: 	Register<typename T::UnderlyingType>(value) {}
	// enum class
	constexpr FlagsGroup(typename T::Type value)
	:	Register<typename T::UnderlyingType>(typename T::UnderlyingType(value)) {}
	// Flags class
	constexpr FlagsGroup(T value)
	:	Register<typename T::UnderlyingType>(value.value) {}
};
/// @endcond


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
	explicit constexpr Value(typename Parent::UnderlyingType config) :
		Register<PType>((config << Position) & Mask) {}
	constexpr Value(Value const &o) :
		Register<PType>(o.value) {}

	constexpr operator Parent() const
	{	return Parent(Register<PType>::value); }

	static constexpr PType get(Parent const &parent)
	{	return (parent.value & Mask) >> Position; }

	static inline void reset(Parent &parent)
	{	parent.value &= ~Mask; }

	static inline void set(Parent &parent, typename Parent::UnderlyingType config)
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
	explicit constexpr Configuration(typename Parent::UnderlyingType config) :
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
/// @endcond


#endif // XPCC_REGISTER_HPP
