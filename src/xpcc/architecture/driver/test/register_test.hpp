// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_UNITTEST_REGISTER_HPP
#define XPCC_UNITTEST_REGISTER_HPP

#include <unittest/testsuite.hpp>
//#include "register_definitions.hpp"


#define REGISTER(type, name, ...) \
	enum class name : type { \
		__VA_ARGS__ }; \
	struct CONCAT(name, _t) : public Register<type> { \
		constexpr CONCAT(name, _t)(name value) \
		:	Register8(static_cast<type>(value)) {} \
		friend constexpr CONCAT(name, _t) operator|(name const &lhs, CONCAT(name, _t) const &rhs) \
		{ return CONCAT(name, _t)(static_cast<name>(static_cast<type>(lhs) | rhs.value)); } \
		friend constexpr CONCAT(name, _t) operator|(CONCAT(name, _t) const &lhs, name const &rhs) \
		{ return CONCAT(name, _t)(static_cast<name>(static_cast<type>(rhs) | lhs.value)); } \
		friend constexpr CONCAT(name, _t) operator|(CONCAT(name, _t) const &lhs, CONCAT(name, _t) const &rhs) \
		{ return CONCAT(name, _t)(static_cast<name>(lhs.value | rhs.value)); } \
	}; \
	constexpr CONCAT(name, _t) operator|(name const &lhs, name const &rhs) \
	{ return CONCAT(name, _t)(static_cast<name>(static_cast<type>(lhs) | static_cast<type>(rhs))); }



template< typename T >
struct Register
{
	typedef T Type;
	T value{0};

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

//REGISTER( uint8_t, Test,
//	A = 0x01,
//	B = 0x02,
//	C = 0x04,
//	D = 0x08,
//	E = 0x10,
//	F = 0x20,
//	G = 0x40,
//	H = 0x80,
//	Mask = 0xff,
//);

REGISTER( uint8_t, Test2,
		A = 0x01,
		B = 0x02,
		C = 0x04,
		D = 0x08,
		E = 0x10,
		F = 0x20,
		G = 0x40,
		H = 0x80,
		Mask = 0xff,
);

enum class
Test : uint8_t
{
	A = 0x01,
	B = 0x02,
	C = 0x04,
	D = 0x08,
	E = 0x10,
	F = 0x20,
	G = 0x40,
	H = 0x80,
	Mask = 0xff,
};

struct
Test_t : public Register8
{
	constexpr Test_t(Test value)
	:	Register8(static_cast<uint8_t>(value)) {}

	friend constexpr Test_t operator|(Test const &lhs, Test_t const &rhs)
	{
		return Test_t(static_cast<Test>(static_cast<uint8_t>(lhs) | rhs.value));
	}

	friend constexpr Test_t operator|(Test_t const &lhs, Test const &rhs)
	{
		return Test_t(static_cast<Test>(static_cast<uint8_t>(rhs) | lhs.value));
	}

	friend constexpr Test_t operator|(Test_t const &lhs, Test_t const &rhs)
	{
		return Test_t(static_cast<Test>(lhs.value | rhs.value));
	}
};

constexpr Test_t operator|(Test const &lhs, Test const &rhs)
{
	return Test_t(static_cast<Test>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs)));
}


//REGISTER8(STest,
//	A = 0x03,
//	B = 0x04,
//	Mask = 0x07
//);


// @author Niklas Hauser
class RegisterTest : public unittest::TestSuite
{
public:
	void
	testOperators();

//	void
//	testExclusiveness();

//	void
//	testGroup();
//
//	void
//	testFunctionOverloading();
};


#endif // XPCC_UNITTEST_REGISTER_HPP
