/*
 * Copyright (c) 2020, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "tmp12x_test.hpp"

void Tmp12xTest::testTmp123TemperatureData()
{
	// Test TMP121, TMP123 temperature conversion (12+1 bit, 1/16 °C resolution)
	// Testcases are example values from the TMP123 datasheet

	// 0xE480, -55 °C
	const modm::Tmp123Temperature temp0{0xE480};
	TEST_ASSERT_EQUALS(temp0.getTemperatureInteger(), -55);
	TEST_ASSERT_EQUALS(temp0.getTemperatureFractional(), -55 * 16);
	TEST_ASSERT_EQUALS_FLOAT(temp0.getTemperatureFloat(), -55.f);
	// 0x4B00, 150 °C
	const modm::Tmp123Temperature temp1{0x4B00};
	TEST_ASSERT_EQUALS(temp1.getTemperatureInteger(), 150);
	TEST_ASSERT_EQUALS(temp1.getTemperatureFractional(), 150 * 16);
	TEST_ASSERT_EQUALS_FLOAT(temp1.getTemperatureFloat(), 150.f);
	// 0x0000, 0 °C
	const modm::Tmp123Temperature temp2{0x0000};
	TEST_ASSERT_EQUALS(temp2.getTemperatureInteger(), 0);
	TEST_ASSERT_EQUALS(temp2.getTemperatureFractional(), 0);
	TEST_ASSERT_EQUALS_FLOAT(temp2.getTemperatureFloat(), 0.f);
	// 0x0000, 0 °C, test 3 invalid lsb are ignored
	const modm::Tmp123Temperature temp3{0x0007};
	TEST_ASSERT_EQUALS(temp3.getTemperatureInteger(), 0);
	TEST_ASSERT_EQUALS(temp3.getTemperatureFractional(), 0);
	TEST_ASSERT_EQUALS_FLOAT(temp3.getTemperatureFloat(), 0.f);
	// 0xFFF8, -0.0625 °C
	const modm::Tmp123Temperature temp4{0xFFF8};
	TEST_ASSERT_EQUALS(temp4.getTemperatureInteger(), 0);
	TEST_ASSERT_EQUALS(temp4.getTemperatureFractional(), -1);
	TEST_ASSERT_EQUALS_FLOAT(temp4.getTemperatureFloat(), -0.0625f);
}

void Tmp12xTest::testTmp125TemperatureData()
{
	// Test TMP125 temperature conversion (10 bit, 1/4 °C resolution)
	// Testcases are example values from the TMP125 datasheet

	// 0x6480, -55 °C
	const modm::Tmp125Temperature temp5{0x6480};
	TEST_ASSERT_EQUALS(temp5.getTemperatureInteger(), -55);
	TEST_ASSERT_EQUALS(temp5.getTemperatureFractional(), -55 * 4);
	TEST_ASSERT_EQUALS_FLOAT(temp5.getTemperatureFloat(), -55.f);
	// 0x3F80, 127 °C
	const modm::Tmp125Temperature temp6{0x3F80};
	TEST_ASSERT_EQUALS(temp6.getTemperatureInteger(), 127);
	TEST_ASSERT_EQUALS(temp6.getTemperatureFractional(), 127 * 4);
	TEST_ASSERT_EQUALS_FLOAT(temp6.getTemperatureFloat(), 127.f);
	// 0x0000, 0 °C
	const modm::Tmp125Temperature temp7{0x0000};
	TEST_ASSERT_EQUALS(temp7.getTemperatureInteger(), 0);
	TEST_ASSERT_EQUALS(temp7.getTemperatureFractional(), 0);
	TEST_ASSERT_EQUALS_FLOAT(temp7.getTemperatureFloat(), 0.f);
	// 0x0000, 0 °C, test bits 15,4-0 are ignored
	const modm::Tmp125Temperature temp8{uint16_t((1 << 15) | 0b11111)};
	TEST_ASSERT_EQUALS(temp8.getTemperatureInteger(), 0);
	TEST_ASSERT_EQUALS(temp8.getTemperatureFractional(), 0);
	TEST_ASSERT_EQUALS_FLOAT(temp8.getTemperatureFloat(), 0.f);
	// 0xFFF8, -0.0625 °C
	const modm::Tmp125Temperature temp9{0x7FE0};
	TEST_ASSERT_EQUALS(temp9.getTemperatureInteger(), 0);
	TEST_ASSERT_EQUALS(temp9.getTemperatureFractional(), -1);
	TEST_ASSERT_EQUALS_FLOAT(temp9.getTemperatureFloat(), -0.25f);
}
