/*
 * Copyright (c) 2014-2016, 2018, Niklas Hauser
 * Copyright (c) 2015-2016, Sascha Schade
 * Copyright (c) 2017, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_REGISTER_HPP
#define MODM_REGISTER_HPP

#include <stdint.h>
#include <modm/math/utils/bit_constants.hpp>
#include <modm/architecture/detect.hpp>
#include <modm/architecture/utils.hpp>
#if MODM_HAS_IOSTREAM
#include <modm/io/iostream.hpp>
#endif

namespace modm
{

/**
 * Class for holding a register value.
 *
 * You can cast any Flags, Configuration or Value type into the
 * approprate Register type.
 *
 * @code
 * Control_t control = Control::EN;
 * Register8 base = control;
 *
 * // functions can also be overloaded to accept *any* register of x bits
 * void function(Register8 arg);
 * function(control);
 * @endcode
 *
 * Apart from the boolean operators, this base class deletes all other
 * implicit operator overloadings.
 * Since any type information is lost during casting, you should not be
 * able to use any operations involving specific flags, configurations or values.
 *
 * Instead you have to explicitly use the integral `value` to perform your
 * potentially unsafe operations.
 *
 * This class makes heavy use of `constexpr`, so that as many values
 * and operation can take place at compile time for efficiency.
 *
 * @tparam	T	the (unsigned) underlying integer type: uint8_t, uint16_t, uint32_t
 * @see Register8, Register16, Register32
 *
 * @ingroup	modm_architecture_register
 * @author	Niklas Hauser
 */
template< typename T >
struct Register
{
	typedef T UnderlyingType;	///< The integer type internally used to represent the register
	UnderlyingType value;		///< The underlying integer value

	/// Default constructor initializes `value` to zero.
	constexpr Register()
	:	value(0) {}

	/**
	 * Returns `true` if `value` is non-zero
	 *
	 * The compiler will allow implicit conversions to bool in the following contexts:
	 * - conditions of if, while, for, do-while statements
	 * - logical operators (&&, ||)
	 * - negation (operator !)
	 * - static_assert
	 */
	explicit constexpr operator bool() const
	{ return bool(value); }

	/// Returns `true` if `value` is zero
	constexpr bool operator!() const
	{ return not bool(value); }

#if MODM_HAS_IOSTREAM
	/// Printing a register will output its numeric value.
	friend IOStream&
	operator << (IOStream& s, const Register<T>& m)
	{ return (s << m.value); }
#endif

protected:
	/// This class is meant to be subclassed
	constexpr Register(UnderlyingType initialValue)
	:	value(initialValue) {}
};

/// @ingroup	modm_architecture_register
/// @{
using Register8  = Register<uint8_t>;	///< Register class with  8-bit wide underlying type
using Register16 = Register<uint16_t>;	///< Register class with 16-bit wide underlying type
using Register32 = Register<uint32_t>;	///< Register class with 32-bit wide underlying type
/// @}


/**
 * Class for operating on a register.
 *
 * This class extends the Register base class by adding constructor
 * and bitwise operation overloading between
 * a strongly-typed enum and itself.
 * It is the common base class for Flags, Configurations and Values!
 *
 * @tparam	Enum	a strongly-typed enum containing the bit masks
 * @tparam	T		the underlying integer type to be for internal representation
 *
 * @see Flags
 *
 * @ingroup	modm_architecture_register
 * @author	Niklas Hauser
 */
template < typename Enum, typename T >
struct FlagsOperators : public ::modm::Register<T>
{
	/// @cond
	typedef T UnderlyingType;
	using Register<T>::value;
	/// @endcond

	typedef Enum EnumType;	///< The enum type

	// MARK: Constructors and assignments

	/// @{

	/// default constructor initializes `value` to zero
	constexpr FlagsOperators() {}

	/// constructs itself out of a strongly-typed enum
	constexpr FlagsOperators(Enum flag) :
		Register<T>(T(flag)) {}

	/// When explicitly called, this class constructs itself out of the underlying type
	explicit constexpr FlagsOperators(UnderlyingType flag) :
		Register<T>(flag) {}

	/// When explicitly called, this class constructs itself out of the superclass
	explicit constexpr FlagsOperators(Register<T> const &o) :
		Register<T>(o.value) {}

	constexpr FlagsOperators(FlagsOperators const &o) = default;	///< Copy constructor
	constexpr FlagsOperators(FlagsOperators &&o) = default;		///< Move constructor
	/// @}

	/// @{
	FlagsOperators& operator=(FlagsOperators const &o) = default;	///< Copy assignment
	FlagsOperators& operator=(FlagsOperators &&o) = default;		///< Move assignment
	/// @}

	static constexpr FlagsOperators mask()	/// returns the mask of the integer type
	{ return FlagsOperators(T(-1)); }

	// flags binary operations
	/// bitwise negation
	constexpr FlagsOperators operator~() const { return FlagsOperators(~value); }

	/// @{

	/// bitwise **AND** with multiple bits
	constexpr FlagsOperators operator&(FlagsOperators const &o) const { return FlagsOperators(value & o.value); }
	/// bitwise **OR** with multiple bits
	constexpr FlagsOperators operator|(FlagsOperators const &o) const { return FlagsOperators(value | o.value); }
	/// bitwise **XOR** with multiple bits
	constexpr FlagsOperators operator^(FlagsOperators const &o) const { return FlagsOperators(value ^ o.value); }
	/// @}

	/// @{

	/// bitwise **AND** with multiple bits
	FlagsOperators& operator&=(FlagsOperators const &o) {  return (value &= o.value, *this); }
	/// bitwise **OR** with multiple bits
	FlagsOperators& operator|=(FlagsOperators const &o) { return (value |= o.value, *this); }
	/// bitwise **XOR** with multiple bits
	FlagsOperators& operator^=(FlagsOperators const &o) { return (value ^= o.value, *this); }
	/// @}

	/// @{

	/// bitwise **AND** with a single bit
	FlagsOperators& operator&=(Enum const &flag) { return operator&=(FlagsOperators(flag)); }
	/// bitwise **OR** with a single bit
	FlagsOperators& operator|=(Enum const &flag) { return operator|=(FlagsOperators(flag)); }
	/// bitwise **XOR** with a single bit
	FlagsOperators& operator^=(Enum const &flag) { return operator^=(FlagsOperators(flag)); }
	/// @}

	/// @{

	/// bitwise **AND** with a single bit
	friend constexpr FlagsOperators operator&(Enum const &a, FlagsOperators const &b) { return b & a; }
	/// bitwise **OR** with a single bit
	friend constexpr FlagsOperators operator|(Enum const &a, FlagsOperators const &b) { return b | a; }
	/// bitwise **XOR** with a single bit
	friend constexpr FlagsOperators operator^(Enum const &a, FlagsOperators const &b) { return b ^ a; }
	/// @}

#ifndef __DOXYGEN__
	constexpr FlagsOperators operator&(Enum const &flag) const { return operator&(FlagsOperators(flag)); }
	constexpr FlagsOperators operator|(Enum const &flag) const { return operator|(FlagsOperators(flag)); }
	constexpr FlagsOperators operator^(Enum const &flag) const { return operator^(FlagsOperators(flag)); }

	constexpr bool operator==(Enum const &flag) const { return value == T(flag); }
	constexpr bool operator!=(Enum const &flag) const { return value != T(flag); }

	friend constexpr bool operator==(Enum const &a, FlagsOperators const &b) { return b == a; }
	friend constexpr bool operator!=(Enum const &a, FlagsOperators const &b) { return b != a; }

	constexpr bool operator==(FlagsOperators const &o) const { return value == o.value; }
	constexpr bool operator!=(FlagsOperators const &o) const { return value != o.value; }
#else
	/// @{

	/// bitwise **AND** with a single bit
	friend constexpr FlagsOperators operator&(FlagsOperators const &a, Enum const &b);
	/// bitwise **OR** with a single bit
	friend constexpr FlagsOperators operator|(FlagsOperators const &a, Enum const &b);
	/// bitwise **XOR** with a single bit
	friend constexpr FlagsOperators operator^(FlagsOperators const &a, Enum const &b);
	/// @}

	/// @{

	/// bitwise **AND** with a single bit
	friend constexpr FlagsOperators operator&(Enum const &a, Enum const &b);
	/// bitwise **OR** with a single bit
	friend constexpr FlagsOperators operator|(Enum const &a, Enum const &b);
	/// bitwise **XOR** with a single bit
	friend constexpr FlagsOperators operator^(Enum const &a, Enum const &b);
	/// @}

	/// Compares if two flags are equal
	/// @{
	friend constexpr bool operator==(FlagsOperators const &a, Enum const &b);
	friend constexpr bool operator!=(FlagsOperators const &a, Enum const &b);
	friend constexpr bool operator==(Enum const &a, FlagsOperators const &b);
	friend constexpr bool operator!=(Enum const &a, FlagsOperators const &b);
	/// @}
#endif
} modm_packed;

/**
 * Class for operating on a register.
 *
 * This class extends the FlagsOperators base class by adding constructor
 * overloading and convenience functions between
 * a strongly-typed enum and itself.
 *
 * This class makes heavy use of `constexpr`, so that as many values
 * and operation can take place at compile time for efficiency.
 *
 * Unfortunately, you cannot typedef this class directly, you must use
 * the `MODM_FLAGS` macro, which add the required external operator overloads
 * for `Enum OP Enum`.
 * The macro `MODM_FLAGS8(Enum)` expands as follows:
 * @code
 * typedef modm::Flags8<Enum> Enum_t;
 * MODM_INT_TYPE_FLAGS(Enum_t);
 * @endcode
 *
 * The `MODM_FLAGS` macro by default only works in a class or struct, as this is
 * the primary use-case in modm.
 * To use it outside of a struct, use this:
 * @code
 * typedef modm::Flags8<Enum> Enum_t;
 * MODM_TYPE_FLAGS(Enum_t);
 * @endcode
 *
 * @tparam	Enum	a strongly-typed enum containing the bit masks
 * @tparam	T		the underlying integer type to be for internal representation
 *
 * @see Flags8, Flags16, Flags32
 * @see MODM_FLAGS8, MODM_FLAGS16, MODM_FLAGS32
 *
 * @ingroup	modm_architecture_register
 * @author	Niklas Hauser
 */
template < typename Enum, typename T >
struct Flags : public ::modm::FlagsOperators<Enum, T>
{
	/// @cond
	typedef T UnderlyingType;
	using FlagsOperators<Enum, T>::value;
	/// @endcond

	typedef Enum EnumType;	///< The enum type

	// MARK: Constructors and assignments

	/// @{

	/// default constructor initializes `value` to zero
	constexpr Flags() {}

	/// constructs itself out of a strongly-typed enum
	constexpr Flags(Enum flag) :
		FlagsOperators<Enum, T>(flag) {}

	constexpr Flags(FlagsOperators<Enum, T> const &flag) :
		FlagsOperators<Enum, T>(flag) {}

	/// When explicitly called, this class constructs itself out of the underlying type
	explicit constexpr Flags(UnderlyingType flag) :
		FlagsOperators<Enum, T>(flag) {}

	/// When explicitly called, this class constructs itself out of the superclass
	explicit constexpr Flags(Register<T> const &o) :
		FlagsOperators<Enum, T>(o) {}

	constexpr Flags(Flags const &o) = default;	///< Copy constructor
	constexpr Flags(Flags &&o) = default;		///< Move constructor
	/// @}

	/// @{
	Flags& operator=(Flags const &o) = default;	///< Copy assignment
	Flags& operator=(Flags &&o) = default;		///< Move assignment
	/// @}

	/// @{
	// MARK: convenience methods
	/// Sets a single bit.
	inline void set(Enum const &flag)
	{ *this |= flag; }
	/// Sets multiple bits.
	inline void set(Flags const &o)
	{ *this |= o; }

	/// Clears a single bit.
	inline void reset(Enum const &flag)
	{ *this &= ~flag; }
	/// Clears multiple bits.
	inline void reset(Flags const &o)
	{ *this &= ~o; }

	/// Toggles a single bit.
	inline void toggle(Enum const &flag)
	{ *this ^= flag; }
	/// Toggles multiple bits.
	inline void toggle(Flags const &o)
	{ *this ^= o; }

	/// Sets a single bit, when `valueToCompare` is true, clears it otherwise
	inline void update(Enum const &flag, bool valueToCompare)
	{
		if (valueToCompare) *this |= flag;
		else *this &= ~flag;
	}
	/// Sets multiple bits, when `valueToCompare` is true, clears them otherwise
	inline void update(Flags const &o, bool valueToCompare)
	{
		if (valueToCompare) *this |= o;
		else *this &= ~o;
	}
	/// @}

	/// @{

	/// Returns `true` if bit is set
	constexpr bool all(Enum const &flag) const
	{ return bool(*this & flag); }
	/// Returns `true` if bit is set
	constexpr bool any(Enum const &flag) const
	{ return bool(*this & flag); }
	/// Returns `true` if bit is **not** set
	constexpr bool none(Enum const &flag) const
	{ return !(*this & flag); }
	/// @}

	/// @{

	/// Returns `true` if **all** of the passed bits are set
	constexpr bool all(Flags const &o) const
	{ return (*this & o) == o; }
	/// Returns `true` if **any** of the passed bits are set
	constexpr bool any(Flags const &o) const
	{ return bool(*this & o); }
	/// Returns `true` if **none** of the passed bits are set
	constexpr bool none(Flags const &o) const
	{ return (*this & o).value == 0; }
	/// @}
} modm_packed;

/// @ingroup	modm_architecture_register
/// @{
template < typename Enum = uint8_t >
using Flags8 = Flags<Enum, uint8_t>;	///< Flags class with  8-bit wide underlying type
template < typename Enum = uint16_t >
using Flags16 = Flags<Enum, uint16_t>;	///< Flags class with 16-bit wide underlying type
template < typename Enum = uint32_t >
using Flags32 = Flags<Enum, uint32_t>;	///< Flags class with 32-bit wide underlying type
/// @}


/// @cond
template < typename... T >
struct FlagsGroup {} modm_packed;


template < typename T, typename... Ts >
struct FlagsGroup<T, Ts...> : public FlagsGroup<Ts...>
{
	// use inherited constructors
	using FlagsGroup<Ts...>::FlagsGroup;

	/// empty constructor initializes `value` to zero
	constexpr FlagsGroup() {};

	// enum class
	constexpr FlagsGroup(typename T::EnumType value)
	:	FlagsGroup<Ts...>(typename T::UnderlyingType(value)) {}
	/// Flags operators and Flags constructor
	constexpr FlagsGroup(::modm::FlagsOperators<typename T::EnumType, typename T::UnderlyingType> value)
	:	FlagsGroup<Ts...>(value.value) {}

} modm_packed;
/// @endcond

/**
 * Class for grouping several Flags classes together.
 *
 * This variadic template class allows you to create a type
 * which provides constructor overloads for the provided types.
 *
 * This will allow you to cast the passed Flags classes into one
 * class, but not other classes.
 * Notice that this cast destroys type information (naturally)
 * and therefore you cannot use any of the Flags classes type-safe
 * operator overloads.
 * The same reasoning applies here as in the Register class.
 *
 * @code
 * typedef Flags8<Control1> Control1_t;
 * typedef Flags8<Control2> Control2_t;
 *
 * typedef FlagsGroup<Control1_t, Control2_t> Control_t;
 *
 * void function(Control_t control);
 *
 * // both call the same function
 * function(Control1_t(0x20));
 * function(Control2_t(0x10));
 * @endcode
 *
 * @tparam	T... the Flags classes to be grouped. Must all have the same underlying types.
 *
 * @ingroup	modm_architecture_register
 * @author	Niklas Hauser
 */
#ifdef __DOXYGEN__
template < typename T... >
struct FlagsGroup<T...> : public Register<T::UnderlyingType>
#else
template < typename T >
struct FlagsGroup<T> : public Register<typename T::UnderlyingType>
#endif
{
	/// default constructor initializes `value` to zero
	constexpr FlagsGroup() {};

	/// copy constructor
	constexpr FlagsGroup(FlagsGroup const &o)
	:	Register<typename T::UnderlyingType>(o.value) {}
	/// explicit underlying type constructor
	explicit constexpr FlagsGroup(typename T::UnderlyingType value)
	: 	Register<typename T::UnderlyingType>(value) {}

	/// enum type constructor
	constexpr FlagsGroup(typename T::EnumType value)
	:	Register<typename T::UnderlyingType>(typename T::UnderlyingType(value)) {}
	/// Flags operators and Flags constructor
	constexpr FlagsGroup(::modm::FlagsOperators<typename T::EnumType, typename T::UnderlyingType> value)
	:	Register<typename T::UnderlyingType>(value.value) {}

} modm_packed;

/**
 * Class for accessing a configuration in a register.
 *
 * A configuration is a combination of bits, whose meaning is different from their
 * numerical value.
 * A typical example is a 2-bit prescaler with the values [0,1,2,3] correspond to
 * the meanings [Div1, Div2, Div4, Div8].
 *
 * The Configuration class belongs to a specific Flags class and connects a
 * corresponding strongly typed enum with a specific mask and bit position.
 * This means that the Configuration class will mask and shift the values whenever
 * needed.
 * However, shifting is only done when the Position template parameter is non-zero.
 *
 * To understand when shifting is necessary, consider the following two scenarios:
 *     1. There is only one prescaler configuration at bit position 4, and
 *     2. There are two or more prescaler configurations at several bit positions.
 *
 * For the first scenario, no shifting is required, as we can declare the prescaler
 * values already in the shifted position:
 * @code
 * enum class
 * Prescaler : uint8_t
 * {
 *     Div1 = (0 << 4),
 *     Div2 = (1 << 4),
 *     Div4 = (2 << 4),
 *     Div8 = (3 << 4),
 * };
 * typedef Configuration<Control_t, Prescaler, (0b11 << 4)> Prescaler_t;
 * @endcode
 *
 * For the second scenario, shifting is required:
 * @code
 * enum class
 * Prescaler : uint8_t
 * {
 *     Div1 = 0,
 *     Div2 = 1,
 *     Div4 = 2,
 *     Div8 = 3,
 * };
 * // shift position 4
 * typedef Configuration<Control_t, Prescaler, 0b11, 4> Prescaler_t;
 * // shift position 6
 * typedef Configuration<Control_t, Prescaler, 0b11, 6> Prescaler2_t;
 * @endcode
 *
 * However, clever choice of the mask and shift position values can minimize
 * shifting.
 * @code
 * enum class
 * Prescaler : uint8_t
 * {
 *     Div1 = (0 << 4),
 *     Div2 = (1 << 4),
 *     Div4 = (2 << 4),
 *     Div8 = (3 << 4),
 * };
 * // shift position 4, not shifted
 * typedef Configuration<Control_t, Prescaler, (0b11 << 4)> Prescaler_t;
 * // shift position 6, shifted only by 2 bits
 * typedef Configuration<Control_t, Prescaler, (0b11 << 4), 2> Prescaler2_t;
 * @endcode
 *
 * This is especially useful on CPUs, which do not have a barrelshifter
 * like the AVRs.
 *
 * @tparam	Parent		the Flags class to which this configuration belongs to
 * @tparam	Enum		a strongly-typed enum containing the configuration values
 * @tparam	Mask		the (unshifted) bit mask corresponding to the enum values
 * @tparam	Position	how many bits the configuration values need to be shifted
 *
 * @ingroup	modm_architecture_register
 * @author	Niklas Hauser
 */
template < typename Parent, typename Enum, typename Parent::UnderlyingType Mask, typename Parent::UnderlyingType Position = 0 >
struct Configuration : public ::modm::FlagsOperators<typename Parent::EnumType, typename Parent::UnderlyingType>
{
private:
	typedef typename Parent::UnderlyingType UnderlyingType;
	typedef UnderlyingType UType;	// lazy
	typedef typename Parent::EnumType EType;	// lazy

public:
	/// explicit constructor for the underlying type
	explicit constexpr Configuration(UnderlyingType config) :
		FlagsOperators<EType, UType>(config << Position) {}
	/// explicit constructor for the enum type
	explicit constexpr Configuration(Enum config) :
		FlagsOperators<EType, UType>(UType(config) << Position) {}

	/// copy constructor
	constexpr Configuration(Configuration const &o) = default;
	/// move constructor
	constexpr Configuration(Configuration &&o) = default;

	/// @{

	/// clears and sets a new configuration in a Flags register
	static inline void set(Parent &parent, Enum config)
	{	parent.value = (parent.value & ~(Mask << Position)) | (UType(config) << Position); }

	/// clears the configuration in a Flags register
	static inline void reset(Parent &parent)
	{	parent.value &= ~(Mask << Position); }

	/// returns the configuration from a Flags register
	static constexpr Enum get(Parent const &parent)
	{	return Enum((parent.value & (Mask << Position)) >> Position); }
	/// @}

	/// returns the shifted mask for this configuration
	static constexpr Parent mask()
	{ return Parent((Mask << Position)); }

	/// @cond
	constexpr operator Parent() const
	{	return Parent(FlagsOperators<EType, UType>::value); }
	/// @endcond
} modm_packed;

/**
 * Class for accessing a numeric value in a register.
 *
 * A value is a combination of bits which correspond to their numerical value.
 * An example is a 3-bit wide bit field corresponding to the start-up delay in ms.
 *
 * The value class is similar to the configuration class, but a lot simpler.
 * No enum type is needed, instead the underlying type of the parent register class
 * is used for setting and getting the value.
 *
 * @code
 * typedef Value< Control_t, 3, 1 >  Delay_t;
 * @endcode
 *
 * @tparam	Parent		the Flags class to which this configuration belongs to
 * @tparam	Width		the width in bits of the value in the register
 * @tparam	Position	how many bits the configuration values need to be shifted
 *
 * @ingroup	modm_architecture_register
 * @author	Niklas Hauser
 */
template < typename Parent, typename Parent::UnderlyingType Width, typename Parent::UnderlyingType Position = 0 >
struct Value : public ::modm::FlagsOperators<typename Parent::EnumType, typename Parent::UnderlyingType>
{
private:
	typedef typename Parent::UnderlyingType UnderlyingType;
	typedef UnderlyingType UType;	// lazy
	typedef typename Parent::EnumType EType;	// lazy

	static constexpr UType Mask = ((1 << Width) - 1) << Position;
public:
	/// explicit constructor for the underlying type
	explicit constexpr Value(UnderlyingType config) :
		FlagsOperators<EType, UType>((config << Position) & Mask) {}

	/// copy constructor
	constexpr Value(Value const &o) = default;
	/// move constructor
	constexpr Value(Value &&o) = default;

	/// @{

	/// clears and sets a new value in a Flags register
	static inline void set(Parent &parent, UnderlyingType config)
	{	parent.value = (parent.value & ~Mask) | ((config << Position) & Mask); }

	/// clears the value in a Flags register
	static inline void reset(Parent &parent)
	{	parent.value &= ~Mask; }

	/// returns the value from a Flags register
	static constexpr UType get(Parent const &parent)
	{	return (parent.value & Mask) >> Position; }
	/// @}

	/// returns the shifted mask for this value
	static constexpr Parent mask()
	{ return Parent(Mask); }

	/// @cond
	constexpr operator Parent() const
	{	return Parent(FlagsOperators<EType, UType>::value); }
	/// @endcond
} modm_packed;

}	// namespace modm

// these operator overloadings will overload *ALL* possible enum classes
/*
template <typename Enum>
constexpr ::modm::Flags<Enum> operator~(Enum const &lhs) { return ~::modm::Flags<Enum>(lhs); }
template <typename Enum>
constexpr ::modm::Flags<Enum> operator|(Enum const &a, Enum const &b) { return ::modm::Flags<Enum>(a) | b;  }
template <typename Enum>
constexpr ::modm::Flags<Enum> operator&(Enum const &a, Enum const &b) { return ::modm::Flags<Enum>(a) & b; }
template <typename Enum>
constexpr ::modm::Flags<Enum> operator^(Enum const &a, Enum const &b) { return ::modm::Flags<Enum>(a) ^ b; }
//*/

/**
 * @details
 * This macro creates a Flags8 class with the enum class provided and
 * adds the required bitwise `(Enum op Enum)` operator overloads for it
 * The resulting Flags8 class type will have the signature `Enum_t`.
 *
 * @warning	This macro only works properly **inside** of a struct or class.
 * @see	MODM_TYPE_FLAGS
 *
 * @param	Enum	a strongly-typed enum containing the bit masks
 *
 * @ingroup	modm_architecture_register
 * @hideinitializer
 */
#define MODM_FLAGS8(Enum) \
	typedef ::modm::Flags8<Enum> MODM_CONCAT(Enum, _t); \
	MODM_INT_TYPE_FLAGS(MODM_CONCAT(Enum, _t))

/**
 * @details
 * This macro creates a Flags16 class with the enum class provided and
 * adds the required bitwise `(Enum op Enum)` operator overloads for it.
 * The resulting Flags16 class type will have the signature `Enum_t`.
 *
 * @warning	This macro only works properly **inside** of a struct or class.
 * @see	MODM_TYPE_FLAGS
 *
 * @param	Enum	a strongly-typed enum containing the bit masks
 *
 * @ingroup modm_architecture_register
 * @hideinitializer
 */
#define MODM_FLAGS16(Enum) \
	typedef ::modm::Flags16<Enum> MODM_CONCAT(Enum, _t); \
	MODM_INT_TYPE_FLAGS(MODM_CONCAT(Enum, _t))

/**
 * @details
 * This macro creates a Flags32 class with the enum class provided and
 * adds the required bitwise `(Enum op Enum)` operator overloads for it.
 * The resulting Flags32 class type will have the signature `Enum_t`.
 *
 * @warning	This macro only works properly **inside** of a struct or class.
 * @see	MODM_TYPE_FLAGS
 *
 * @param	Enum	a strongly-typed enum containing the bit masks
 *
 * @ingroup modm_architecture_register
 * @hideinitializer
 */
#define MODM_FLAGS32(Enum) \
	typedef ::modm::Flags32<Enum> MODM_CONCAT(Enum, _t); \
	MODM_INT_TYPE_FLAGS(MODM_CONCAT(Enum, _t))

/**
 * @details
 * This macro creates bitwise `(Parent::EnumType op Parent::EnumType)` operator
 * overloads for use with the Flags class.
 *
 * @warning	This macro only works properly **outside** of a struct or class.
 *
 * @param	Parent	a Flags class
 *
 * @ingroup modm_architecture_register
 * @hideinitializer
 */
#define MODM_TYPE_FLAGS(Parent) \
	MODM_INTERNAL_FLAGS(Parent,)

/**
 * @details
 * This macro creates a `Config_t` type out of the `Parent` and `Config` enum.
 * The mask is taken from a `Parent::Config_Mask` field containing the ORed bits
 * of the configuration.
 *
 * @note This macro does not allow using the configuration position.
 *       Please use the `modm::Configuration` class manually in that case.
 *
 * @param	Parent	a Flags Enum
 * @param	Config	a Config Enum
 *
 * @ingroup modm_architecture_register
 * @hideinitializer
 */
#define MODM_FLAGS_CONFIG(Parent, Config) \
	using MODM_CONCAT(Config, _t) = \
		::modm::Configuration< MODM_CONCAT(Parent, _t), Config, \
		MODM_CONCAT(Parent, _t)::UnderlyingType(MODM_CONCAT(Parent::Config, _Mask)) >

/// @cond
#define MODM_INT_TYPE_FLAGS(Parent) \
	MODM_INTERNAL_FLAGS(Parent, friend)

#define MODM_INTERNAL_FLAGS(Parent, scope) \
	scope constexpr Parent operator compl (typename Parent::EnumType const &lhs) \
	{ return compl Parent(lhs); } \
	MODM_INTERNAL_FLAGS_EOP(type, Parent, &, scope) \
	MODM_INTERNAL_FLAGS_EOP(type, Parent, |, scope) \
	MODM_INTERNAL_FLAGS_EOP(type, Parent, ^, scope)

#define MODM_INTERNAL_FLAGS_EOP(type, Parent, op, scope) \
	scope constexpr Parent operator op (typename Parent::EnumType const &lhs, typename Parent::EnumType const &rhs) \
	{ \
		using EnumType = typename Parent::EnumType; \
		using UnderlyingType = typename Parent::UnderlyingType; \
		return Parent(EnumType(UnderlyingType(lhs) op UnderlyingType(rhs))); \
	}
/// @endcond


#endif // MODM_REGISTER_HPP
