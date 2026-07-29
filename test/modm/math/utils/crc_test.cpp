/*
 * Copyright (c) 2026, Tudor Fanaru
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/utils/crc.hpp>

#include "crc_test.hpp"

namespace
{
	// "123456789" is the input the CRC catalogue publishes check values against,
	// so the expectations below are external references.
	const uint8_t check_input[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
}

void
CrcTest::testCrc8CcittCheckValue()
{
	// CRC-8 with polynomial 0x07 and init 0xFF.
	TEST_ASSERT_EQUALS(modm::math::crc8_ccitt(check_input, sizeof(check_input)), 0xFBU);
}

void
CrcTest::testCrc8CcittUsesEveryInputBit()
{
	// Regression: shifting before testing bit 7 discards it, leaving the MSB of
	// every input byte unexamined. That made crc8_ccitt_update(crc, d) equal
	// crc8_ccitt_update(crc, d ^ 0x80) for all 32768 (crc, d) pairs, put only
	// 128 of 256 outputs in reach, and left single-bit errors in that position
	// undetectable.
	for (uint16_t crc = 0; crc < 256; ++crc)
	{
		for (uint16_t data = 0; data < 128; ++data)
		{
			const uint8_t a = modm::math::crc8_ccitt_update(uint8_t(crc), uint8_t(data));
			const uint8_t b = modm::math::crc8_ccitt_update(uint8_t(crc), uint8_t(data ^ 0x80));
			TEST_ASSERT_TRUE(a != b);
		}
	}
}

void
CrcTest::testCrc16CcittCheckValue()
{
	// CRC-16/MCRF4XX: reflected polynomial 0x8408, init 0xFFFF, no final xor.
	TEST_ASSERT_EQUALS(modm::math::crc16_ccitt(check_input, sizeof(check_input)), 0x6F91U);
}

void
CrcTest::testCrc32CheckValue()
{
	// CRC-32/ISO-HDLC, as used by zlib and PNG.
	TEST_ASSERT_EQUALS(modm::math::crc32(check_input, sizeof(check_input)), 0xCBF43926UL);
}
