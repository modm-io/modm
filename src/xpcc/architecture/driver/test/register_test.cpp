// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "register_test.hpp"

REGISTER8( Test,
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

void
RegisterTest::testOperators()
{
	Test vD;
	vD = Test::A | Test::B;
	TEST_ASSERT_EQUALS(vD.value, 0b11);

	vD = vD | Test::C;
	TEST_ASSERT_EQUALS(vD.value, 0b111);

	vD = Test::D | vD;
	TEST_ASSERT_EQUALS(vD.value, 0b1111);

	Test vI;
	vI = Test::E;
	vD = vD | vI;
	TEST_ASSERT_EQUALS(vD.value, 0b11111);

	// simple raw access is possible
	xpcc::register8_t raw;
	raw = vD;

	TEST_ASSERT_EQUALS(bool(raw), true);
	TEST_ASSERT_TRUE(raw);
	raw = 0;
	TEST_ASSERT_EQUALS(bool(raw), false);
	TEST_ASSERT_FALSE(raw);
}

