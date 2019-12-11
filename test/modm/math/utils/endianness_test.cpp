/*
 * Copyright (c) 2015-2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/utils/endianness.hpp>

#include "endianness_test.hpp"

/**
 * Test coverage is only 50% as this is expected to run on
 * a little Endian system (x86)
 */

void
EndiannessTest::testToBigEndian()
{
	if (modm::isLittleEndian())
	{
		uint32_t val = 0xdeadbeef;
		TEST_ASSERT_EQUALS(modm::toBigEndian(val), 0xefbeadde);

		TEST_ASSERT_EQUALS(modm::toBigEndian(static_cast<uint32_t>(0xdeadbeef)), 0xefbeadde);

		// Constants are promoted to 32 bits by the compiler.
		// Without static cast, this does not yield to the expected result
		TEST_ASSERT_EQUALS(modm::toBigEndian(static_cast<uint16_t>(0xdead)), 0xadde);

		TEST_ASSERT_EQUALS(modm::toBigEndian(static_cast<int32_t>(0xdeadbeef)), static_cast<int32_t>(0xefbeadde));
		TEST_ASSERT_EQUALS(modm::toBigEndian(static_cast<int16_t>(    0xbeef)), static_cast<int16_t>(0xefbe));
	}
}

void
EndiannessTest::testToLittleEndian()
{
	if (modm::isLittleEndian())
	{
		TEST_ASSERT_EQUALS(modm::toLittleEndian(static_cast<uint32_t>(0xdeadbeef)), static_cast<uint32_t>(0xdeadbeef));
		TEST_ASSERT_EQUALS(modm::toLittleEndian(static_cast<uint16_t>(0xdead    )), static_cast<uint16_t>(0xdead    ));
		TEST_ASSERT_EQUALS(modm::toLittleEndian(static_cast<int32_t> (0xdeadbeef)), static_cast<int32_t> (0xdeadbeef));
		TEST_ASSERT_EQUALS(modm::toLittleEndian(static_cast<int16_t> (    0xbeef)), static_cast<int16_t> (0xbeef    ));
	}
}

void
EndiannessTest::testFromBigEndian()
{
	if (modm::isLittleEndian())
	{
		TEST_ASSERT_EQUALS(modm::fromBigEndian(static_cast<uint32_t>(0xdeadbeef)), static_cast<uint32_t>(0xefbeadde));
		TEST_ASSERT_EQUALS(modm::fromBigEndian(static_cast<uint16_t>(0xdead    )), static_cast<uint16_t>(0xadde    ));
		TEST_ASSERT_EQUALS(modm::fromBigEndian(static_cast<int32_t> (0xdeadbeef)), static_cast<int32_t> (0xefbeadde));
		TEST_ASSERT_EQUALS(modm::fromBigEndian(static_cast<int16_t> (    0xbeef)), static_cast<int16_t> (0xefbe    ));
	}
}

void
EndiannessTest::testFromLittleEndian()
{
	if (modm::isLittleEndian())
	{
		TEST_ASSERT_EQUALS(modm::fromLittleEndian(static_cast<uint32_t>(0xdeadbeef)), static_cast<uint32_t>(0xdeadbeef));
		TEST_ASSERT_EQUALS(modm::fromLittleEndian(static_cast<uint16_t>(0xdead    )), static_cast<uint16_t>(0xdead    ));
		TEST_ASSERT_EQUALS(modm::fromLittleEndian(static_cast<int32_t> (0xdeadbeef)), static_cast<int32_t> (0xdeadbeef));
		TEST_ASSERT_EQUALS(modm::fromLittleEndian(static_cast<int16_t> (    0xbeef)), static_cast<int16_t> (0xbeef    ));
	}
}
