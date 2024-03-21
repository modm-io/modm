/*
 * Copyright (c) 2023, Rasmus Kleist Hørlyck Sørensen
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/math/utils/bcd.hpp>

#include "bcd_test.hpp"

void
BcdTest::testFromBcd()
{
	TEST_ASSERT_EQUALS(modm::fromBcd(0x01), 0x01U);
	TEST_ASSERT_EQUALS(modm::fromBcd(0x02), 0x02U);
	TEST_ASSERT_EQUALS(modm::fromBcd(0x03), 0x03U);
	TEST_ASSERT_EQUALS(modm::fromBcd(0x04), 0x04U);
	TEST_ASSERT_EQUALS(modm::fromBcd(0x05), 0x05U);
	TEST_ASSERT_EQUALS(modm::fromBcd(0x06), 0x06U);
	TEST_ASSERT_EQUALS(modm::fromBcd(0x07), 0x07U);
	TEST_ASSERT_EQUALS(modm::fromBcd(0x08), 0x08U);
	TEST_ASSERT_EQUALS(modm::fromBcd(0x09), 0x09U);
}

void
BcdTest::testToBcd()
{
	TEST_ASSERT_EQUALS(modm::toBcd(0x01), 0x01U);
	TEST_ASSERT_EQUALS(modm::toBcd(0x02), 0x02U);
	TEST_ASSERT_EQUALS(modm::toBcd(0x03), 0x03U);
	TEST_ASSERT_EQUALS(modm::toBcd(0x04), 0x04U);
	TEST_ASSERT_EQUALS(modm::toBcd(0x05), 0x05U);
	TEST_ASSERT_EQUALS(modm::toBcd(0x06), 0x06U);
	TEST_ASSERT_EQUALS(modm::toBcd(0x07), 0x07U);
	TEST_ASSERT_EQUALS(modm::toBcd(0x08), 0x08U);
	TEST_ASSERT_EQUALS(modm::toBcd(0x09), 0x09U);
}