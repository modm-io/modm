// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "register_test.hpp"

void
RegisterTest::testAssignments()
{
	// empty assignment
	Test_t v0;
	TEST_ASSERT_EQUALS(v0.value, 0);
	Common_t c0;
	TEST_ASSERT_EQUALS(c0.value, 0);
	xpcc::Register8 r0;
	TEST_ASSERT_EQUALS(r0.value, 0);

	// to Test struct
	v0 = Test::A;
	TEST_ASSERT_EQUALS(v0.value, 0b1);
	Test_t v1 = Test::A;
	TEST_ASSERT_EQUALS(v1.value, 0b1);
	Test_t v2 = v1;
	TEST_ASSERT_EQUALS(v2.value, 0b1);

	// to common struct
	c0 = Test::A;
	TEST_ASSERT_EQUALS(c0.value, 0b1);
	Common_t c1 = Test::A;
	TEST_ASSERT_EQUALS(c1.value, 0b1);
	Common_t c2 = v2;
	TEST_ASSERT_EQUALS(c2.value, 0b1);

	// to register

	// these are not possible!
	//r0 = Test::A;
	//TEST_ASSERT_EQUALS(r0.value, 0b1);
	//xpcc::Register8 r1 = Test::A;
	//TEST_ASSERT_EQUALS(r1.value, 0b1);

	xpcc::Register8 r2 = v2;
	TEST_ASSERT_EQUALS(r2.value, 0b1);

	// to raw value
	uint8_t u1 = v1.value;
	TEST_ASSERT_EQUALS(u1, 0b1);
	uint8_t u2 = c2.value;
	TEST_ASSERT_EQUALS(u2, 0b1);
}


void
RegisterTest::testOperators()
{
	// enum op enum
	Test_t v1;
	v1 = Test::A | Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	v1 = Test::A & Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b00);

	v1 = Test::A ^ Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	v1 = ~Test::D;
	TEST_ASSERT_EQUALS(v1.value, 0b11110111);

	v1 = Test::A | Test::B;
	v1 |= Test::C;
	TEST_ASSERT_EQUALS(v1.value, 0b111);

	v1 &= Test::C;
	TEST_ASSERT_EQUALS(v1.value, 0b100);

	v1 ^= Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b110);


	Test_t v2 = Test::A;
	Test_t v3 = Test::B;
	v1 = v2 | Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	v1 = v2 & Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b00);

	v1 = v2 ^ Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	v1 = v2 | v3;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	v1 = v2 & v3;
	TEST_ASSERT_EQUALS(v1.value, 0b00);

	v1 = v2 ^ v3;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	v1 = ~v2;
	TEST_ASSERT_EQUALS(v1.value, 0b11111110);

	v1 = Test::A | Test::B;
	v2 = Test::C;
	v1 |= v2;
	TEST_ASSERT_EQUALS(v1.value, 0b111);

	v1 &= v2;
	TEST_ASSERT_EQUALS(v1.value, 0b100);

	v1 ^= v2;
	TEST_ASSERT_EQUALS(v1.value, 0b000);
}
