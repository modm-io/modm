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
#include "register_definitions.hpp"


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

enum class
Test : uint8_t
{
	A = 0x03,
	B = 0x04,
	Mask = 0x07,
};

constexpr Test operator|(Test const &lhs, Test const &rhs)
{
	return static_cast<Test>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

struct STest
{
	uint8_t value;
	constexpr STest(uint8_t value)
	:	value(value) {}

	constexpr operator uint8_t() const
	{ return value; }

	enum : uint8_t
	{
		A = 0x03,
		B = 0x04,
		Mask = 0x07
	};
};

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
	testOperators()
	{

	}

	void
	testExclusiveness();

//	void
//	testGroup();
//
//	void
//	testFunctionOverloading();
};


#endif // XPCC_UNITTEST_REGISTER_HPP
