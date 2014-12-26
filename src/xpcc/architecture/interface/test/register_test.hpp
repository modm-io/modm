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
#include <xpcc/architecture/interface/register.hpp>

namespace xpcc
{

struct testing
{
protected:
	enum class
	Test
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
	REGISTER8(Test);

	enum class
	Test2
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
	REGISTER8(Test2);

	REGISTER8_GROUP(Common, Test, Test2);
};

enum class
Test3
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
// test macro outside of struct
// all enum operations must not be declared 'friend'
INTERNAL_REGISTER(uint8_t, Test3, );

}

// @author Niklas Hauser
class RegisterTest : public unittest::TestSuite, public xpcc::testing
{
public:
	void
	testAssignments();

	void
	testOperators();

	void
	testCasting();
};

#endif // XPCC_UNITTEST_REGISTER_HPP
