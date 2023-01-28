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

void
EndiannessTest::testBigEndianStruct()
{
	constexpr uint16_t val16 = 0xdead;
	constexpr uint16_t swap16 = 0xadde;
	constexpr uint32_t val32 = 0xdeadbeef;
	constexpr uint32_t swap32 = 0xefbeadde;

	constexpr modm::BigEndian<uint16_t> be16 = val16;
	constexpr modm::BigEndian<uint32_t> be32 = val32;

	TEST_ASSERT_EQUALS(be16.raw, modm::isBigEndian() ? val16 : swap16);
	TEST_ASSERT_EQUALS(be32.raw, modm::isBigEndian() ? val32 : swap32);
	TEST_ASSERT_EQUALS(be16, val16);
	TEST_ASSERT_EQUALS(be32, val32);
}

void
EndiannessTest::testLittleEndianStruct()
{
	constexpr uint16_t val16 = 0xdead;
	constexpr uint16_t swap16 = 0xadde;
	constexpr uint32_t val32 = 0xdeadbeef;
	constexpr uint32_t swap32 = 0xefbeadde;

	constexpr modm::LittleEndian<uint16_t> le16 = val16;
	constexpr modm::LittleEndian<uint32_t> le32 = val32;

	TEST_ASSERT_EQUALS(le16.raw, modm::isLittleEndian() ? val16 : swap16);
	TEST_ASSERT_EQUALS(le32.raw, modm::isLittleEndian() ? val32 : swap32);
	TEST_ASSERT_EQUALS(le16, val16);
	TEST_ASSERT_EQUALS(le32, val32);
}

void
EndiannessTest::testBigEndianReinterpreted()
{
	struct DataBE {
		modm::BigEndian<uint16_t> be16;
		modm::BigEndian<uint32_t> be32;
	};
	struct DataRaw {
		uint16_t raw16;
		uint32_t raw32;
	};

	DataBE dataBE = {0x0203, 0x04050607};
	DataRaw* dataRaw = reinterpret_cast<DataRaw*>(&dataBE);

	TEST_ASSERT_EQUALS(dataRaw->raw16, modm::isBigEndian() ? 0x0203u : 0x0302u);
	TEST_ASSERT_EQUALS(dataRaw->raw32, modm::isBigEndian() ? 0x04050607u : 0x07060504u);

	dataRaw->raw16 = 0xA1A2;
	dataRaw->raw32 = 0xB1B2B3B4;

	TEST_ASSERT_EQUALS(dataBE.be16, modm::isBigEndian() ? 0xA1A2u : 0xA2A1u);
	TEST_ASSERT_EQUALS(dataBE.be32, modm::isBigEndian() ? 0xB1B2B3B4u : 0xB4B3B2B1u);
}

void
EndiannessTest::testLittleEndianReinterpreted()
{
	struct DataLE {
		modm::LittleEndian<uint16_t> le16;
		modm::LittleEndian<uint32_t> le32;
	};
	struct DataRaw {
		uint16_t raw16;
		uint32_t raw32;
	};

	DataLE dataLE = {0x0203, 0x04050607};
	DataRaw* dataRaw = reinterpret_cast<DataRaw*>(&dataLE);

	TEST_ASSERT_EQUALS(dataRaw->raw16, modm::isLittleEndian() ? 0x0203u : 0x0302u);
	TEST_ASSERT_EQUALS(dataRaw->raw32, modm::isLittleEndian() ? 0x04050607u : 0x07060504u);

	dataRaw->raw16 = 0xA1A2;
	dataRaw->raw32 = 0xB1B2B3B4;

	TEST_ASSERT_EQUALS(dataLE.le16, modm::isLittleEndian() ? 0xA1A2u : 0xA2A1u);
	TEST_ASSERT_EQUALS(dataLE.le32, modm::isLittleEndian() ? 0xB1B2B3B4u : 0xB4B3B2B1u);
}
