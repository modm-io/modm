/*
 * Copyright (c) 2014, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
	modm::Register8 r0;
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
	Common_t c3(0x42);
	TEST_ASSERT_EQUALS(c3.value, 0x42);

	uint8_t a0 = translateCommonArgument(Test::A);
	TEST_ASSERT_EQUALS(a0, 0b1);
	a0 = translateCommonArgument(Test2::A);
	TEST_ASSERT_EQUALS(a0, 0b1);
	a0 = translateCommonArgument(v2);
	TEST_ASSERT_EQUALS(a0, 0b1);
	a0 = translateCommonArgument(v1);
	TEST_ASSERT_EQUALS(a0, 0b1);
	a0 = translateCommonArgument(c3);
	TEST_ASSERT_EQUALS(a0, 0x42);

	// flags operators to common struct
	v0 = Test::A;
	c0 = v0 | Test::C;
	TEST_ASSERT_EQUALS(c0.value, 0b101);

	// to register

	// these are not possible!
	//r0 = Test::A;
	//TEST_ASSERT_EQUALS(r0.value, 0b1);
	//modm::Register8 r1 = Test::A;
	//TEST_ASSERT_EQUALS(r1.value, 0b1);

	modm::Register8 r2 = v2;
	TEST_ASSERT_EQUALS(r2.value, 0b1);

	// to raw value
	uint8_t u1 = v1.value;
	TEST_ASSERT_EQUALS(u1, 0b1);
	uint8_t u2 = c2.value;
	TEST_ASSERT_EQUALS(u2, 0b1);
}

uint8_t
RegisterTest::translateCommonArgument(Common_t common)
{
	return common.value;
}

void
RegisterTest::testOperators()
{
	// flags = enum op enum
	Test_t v1;
	v1 = Test::A | Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	v1 = Test::A & Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b00);

	v1 = Test::A ^ Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	// flags = ~enum
	v1 = ~Test::D;
	TEST_ASSERT_EQUALS(v1.value, 0b11110111);

	v1 = Test::A | Test::B;
	// flags op= enum
	v1 |= Test::C;
	TEST_ASSERT_EQUALS(v1.value, 0b111);

	v1 &= Test::C;
	TEST_ASSERT_EQUALS(v1.value, 0b100);

	v1 ^= Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b110);


	Test_t v2 = Test::A;
	Test_t v3 = Test::B;
	// flags = flags op enum
	v1 = v2 | Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b11);
	v1 = Test::B | v2;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	v1 = v2 & Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b00);

	v1 = v2 ^ Test::B;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	// flags = flags op flags
	v1 = v2 | v3;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	v1 = v2 & v3;
	TEST_ASSERT_EQUALS(v1.value, 0b00);

	v1 = v2 ^ v3;
	TEST_ASSERT_EQUALS(v1.value, 0b11);

	// flags = ~flags
	v1 = ~v2;
	TEST_ASSERT_EQUALS(v1.value, 0b11111110);

	v1 = Test::A | Test::B;
	v2 = Test::C;
	// flags op= flags
	v1 |= v2;
	TEST_ASSERT_EQUALS(v1.value, 0b111);

	v1 &= v2;
	TEST_ASSERT_EQUALS(v1.value, 0b100);

	v1 ^= v2;
	TEST_ASSERT_EQUALS(v1.value, 0b000);

	// comparator
	v1 = Test::A;
	v2 = Test::C | Test::A;
	v3 = Test::A;
	// compare is false
	TEST_ASSERT_FALSE(v1 == v2);
	TEST_ASSERT_FALSE(v1 == Test::B);
	TEST_ASSERT_FALSE(Test::B == v1);
	TEST_ASSERT_FALSE(v1 == (Test::B | Test::A));
	TEST_ASSERT_FALSE((Test::B | Test::A) == v1);

	// compl compare is true
	TEST_ASSERT_TRUE(v1 != v2);
	TEST_ASSERT_TRUE(v1 != Test::B);
	TEST_ASSERT_TRUE(Test::B != v1);
	TEST_ASSERT_TRUE(v1 != (Test::B | Test::A));
	TEST_ASSERT_TRUE((Test::B | Test::A) != v1);

	// compare successful
	TEST_ASSERT_TRUE(v1 == Test::A);
	TEST_ASSERT_TRUE(Test::A == v1);
	TEST_ASSERT_TRUE(v2 == (Test::A | Test::C));
	TEST_ASSERT_TRUE((Test::A | Test::C) == v2);
	TEST_ASSERT_TRUE(v1 == v3);

	TEST_ASSERT_FALSE(v1 != Test::A);
	TEST_ASSERT_FALSE(Test::A != v1);
	TEST_ASSERT_FALSE(v2 != (Test::A | Test::C));
	TEST_ASSERT_FALSE((Test::A | Test::C) != v2);
	TEST_ASSERT_FALSE(v1 != v3);

	// bool conversion
	v1 = Test::A;
	TEST_ASSERT_TRUE(v1);
	v1 = Test(0);
	TEST_ASSERT_FALSE(v1);
}

void
RegisterTest::testFunctions()
{
	Test_t v1;
	Test_t v2 = Test::H;

	v1.set(Test::A);
	TEST_ASSERT_EQUALS(v1.value, 0b1);

	v1.set(Test::B | Test::C);
	TEST_ASSERT_EQUALS(v1.value, 0b111);

	v1.set(v2);
	TEST_ASSERT_EQUALS(v1.value, 0b111 | 0x80);

	v1.reset(v2);
	TEST_ASSERT_EQUALS(v1.value, 0b111);

	v1.reset(Test::B | Test::C);
	TEST_ASSERT_EQUALS(v1.value, 0b1);

	v1.reset(Test::A);
	TEST_ASSERT_EQUALS(v1.value, 0);

	v1.update(Test::A, true);
	TEST_ASSERT_EQUALS(v1.value, 0b1);

	v1.update(Test::B, false);
	TEST_ASSERT_EQUALS(v1.value, 0b1);

	v1.update(v2, false);
	TEST_ASSERT_EQUALS(v1.value, 0b1);

	v1.update(v2, true);
	TEST_ASSERT_EQUALS(v1.value, 0b1 | 0x80);

	v1 = Test::A;
	TEST_ASSERT_TRUE(v1.all(Test::A));
	TEST_ASSERT_FALSE(v1.all(Test::B));
	TEST_ASSERT_TRUE(v1.none(Test::B));

	TEST_ASSERT_FALSE(v1.all(v2));
	TEST_ASSERT_TRUE(v1.none(v2));
	v1 = v2;
	TEST_ASSERT_TRUE(v1.all(v2));

	v1 = Test::A | Test::B | Test::C;
	TEST_ASSERT_TRUE(v1.all(Test::A));
	TEST_ASSERT_TRUE(v1.all(Test::A | Test::B));
	TEST_ASSERT_TRUE(v1.any(Test::A | Test::D));
	TEST_ASSERT_FALSE(v1.all(Test::A | Test::D));
	TEST_ASSERT_TRUE(v1.none(Test::E | Test::D));
	TEST_ASSERT_FALSE(v1.none(Test::A | Test::D));

	TEST_ASSERT_EQUALS(v1.mask().value, 0xff);
	TEST_ASSERT_EQUALS(Test_t::mask().value, 0xff);
}

void
RegisterTest::testCasting()
{
	Test_t v1 = static_cast<Test>(0xff);
	TEST_ASSERT_EQUALS(v1.value, 0xff);
	v1 = Test(0xee);
	TEST_ASSERT_EQUALS(v1.value, 0xee);
	v1 = Test_t(0xdd);
	TEST_ASSERT_EQUALS(v1.value, 0xdd);

	Test_t v2(0xff);
	TEST_ASSERT_EQUALS(v2.value, 0xff);

	Common_t c1 = Common_t(0xff);
	TEST_ASSERT_EQUALS(c1.value, 0xff);
}

void
RegisterTest::testConfigurations()
{
	Test3_t v1 = Config0(Config::One);
	TEST_ASSERT_EQUALS(v1.value, 0b01);

	v1 = Direct0(Direct::One);
	TEST_ASSERT_EQUALS(v1.value, 0b0100);

	v1 = Test3::Bit | Config0(Config::Two);
	TEST_ASSERT_EQUALS(v1.value, (modm::Bit4 | 0b10));

	v1 = Config0(Config::Two) | Test3::Bit;
	TEST_ASSERT_EQUALS(v1.value, (modm::Bit4 | 0b10));

	v1 = Config2(Config::Three) | Config0(Config::Two);
	TEST_ASSERT_EQUALS(v1.value, ((0b11 << 2) | 0b10));

	v1 = Test3::Bit | Config2(Config::Three) | Config0(Config::Two);
	TEST_ASSERT_EQUALS(v1.value, (modm::Bit4 | (0b11 << 2) | 0b10));

	Config2::reset(v1);
	TEST_ASSERT_EQUALS(v1.value, modm::Bit4 | 0b10);

	v1 |= Config2(Config::One);
	TEST_ASSERT_EQUALS(v1.value, (modm::Bit4 | (0b01 << 2) | 0b10));

	Config0::set(v1, Config::Three);
	TEST_ASSERT_EQUALS(v1.value, modm::Bit4 | (0b01 << 2) | 0b11);

	Config2::set(v1, Config::Two);
	TEST_ASSERT_EQUALS(v1.value, modm::Bit4 | (0b10 << 2) | 0b11);

	Config current = Config2::get(v1);
	TEST_ASSERT_EQUALS(uint8_t(current), uint8_t(Config::Two));

	TEST_ASSERT_EQUALS(Config0::mask(), Test3_t(0b11));
	TEST_ASSERT_EQUALS(Config2::mask(), Test3_t(0b11 << 2));
	TEST_ASSERT_EQUALS(Direct0::mask(), Test3_t(0b1100));
}

void
RegisterTest::testValue()
{
	Test3_t v1 = Test3::Bit | Config0(Config::Two) | Address(6);
	TEST_ASSERT_EQUALS(v1.value, (modm::Bit4 | 0b10 | (6 << 5)));

	v1 = Config0(Config::Two) | Address(6) | Test3::Bit;
	TEST_ASSERT_EQUALS(v1.value, (modm::Bit4 | 0b10 | (6 << 5)));

	v1 = Address(6) | Config0(Config::Two) | Test3::Bit;
	TEST_ASSERT_EQUALS(v1.value, (modm::Bit4 | 0b10 | (6 << 5)));

	Address::reset(v1);
	TEST_ASSERT_EQUALS(v1.value, (modm::Bit4 | 0b10));

	v1 |= Address(5);
	TEST_ASSERT_EQUALS(v1.value, modm::Bit4 | 0b10 | (5 << 5));

	Address::set(v1, 1);
	TEST_ASSERT_EQUALS(v1.value, modm::Bit4 | 0b10 | (1 << 5));

	uint8_t addr = Address::get(v1);
	TEST_ASSERT_EQUALS(addr, 1);

	TEST_ASSERT_EQUALS(Address::mask(), Test3_t(7 << 5));
}
