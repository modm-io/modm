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
	XPCC_FLAGS8(Test);

	enum class
	Test2 : uint8_t
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
	XPCC_FLAGS8(Test2);

	XPCC_FLAGS_GROUP(Common_t, Test_t, Test2_t);


	enum class
	Test3 : uint8_t
	{
		// Field1 : 5:7
		Bit = Bit4,
		// Config2 2:3
		// Config0 0:1
	};
	// this configuration field is available once in
	// - bit position 0, and once in
	// - bit position 2.
	enum class
	Config : uint8_t
	{
		Zero = 0b00,
		One = 0b01,
		Two = 0b10,
		Three = 0b11,
	};

	typedef Flags8<Test3> Test3_t;
	XPCC_TYPE_FLAGS(Test3_t);

	typedef Value< Test3_t, 3, 5 > Address;
	typedef Configuration< Test3_t, Config, 2, 2 > Config2;
	typedef Configuration< Test3_t, Config, 2 > Config0;

	typedef Flags8<> Test5_t;
};

enum class
Test4
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
typedef Flags8<Test4> Test4_t;
// test macro outside of struct
// all enum operations must not be declared 'friend'
XPCC_EXT_TYPE_FLAGS(Test4_t)

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
	testFunctions();

	void
	testCasting();

	void
	testConfigurations();

	void
	testValue();
};

#endif // XPCC_UNITTEST_REGISTER_HPP
