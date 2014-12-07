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
RegisterTest::testOperators()
{
	Test_t vD = Test::A | Test::B;
	TEST_ASSERT_EQUALS(vD.value, 0b11);

	vD = vD | Test::C;
	TEST_ASSERT_EQUALS(vD.value, 0b111);

	vD = Test::D | vD;
	TEST_ASSERT_EQUALS(vD.value, 0b1111);

	Test_t vI = Test::E;
	vD = vD | vI;
	TEST_ASSERT_EQUALS(vD.value, 0b11111);

	// simple raw access is possible
	uint8_t raw = vD.value;
	(void) raw;

	// this is possible too
	Register8 reg = vD;
	// and then you cannot modify it anymore, which makes sense
	// since you don't actually know which register this is)
//	reg = reg | Test::A;
	(void) reg;

	// but this is *not possible*, but it must be possible, otherwise it makes no sense.
//	reg = Test::A;
	// neither is extracting the enum class value directly
//	uint8_t raw2 = Test::A.value;

}

