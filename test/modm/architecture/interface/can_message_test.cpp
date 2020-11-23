/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "can_message_test.hpp"

void
CanMessageTest::testEqualOperator()
{
	// Construct two equal messages

	modm::can::Message msgA(0x12);
	msgA.setLength(2);
	msgA.data[0] = 0x82;
	msgA.data[1] = 0x22;

	modm::can::Message msgB(0x12);
	msgB.setLength(2);
	msgB.data[0] = 0x82;
	msgB.data[1] = 0x22;

	TEST_ASSERT_EQUALS(msgA, msgB);
	TEST_ASSERT_TRUE(msgA == msgB);

	// Change Extended Bit
	msgB.setExtended(false);
	TEST_ASSERT_FALSE(msgA == msgB);

	msgB.setExtended(true);
	TEST_ASSERT_TRUE(msgA == msgB);

	// Change a unused data byte
	msgB.data[2] = 0xff;
	TEST_ASSERT_TRUE(msgA == msgB);

	// Change a used data byte
	msgB.data[1] = 0x11;
	TEST_ASSERT_FALSE(msgA == msgB);
}

void
CanMessageTest::testConstructor()
{
	modm::can::Message msgA(0x123, 7, 0x19821122aaffEE);
	TEST_ASSERT_EQUALS(msgA.data[0], 0x19);
	TEST_ASSERT_EQUALS(msgA.data[1], 0x82);
	TEST_ASSERT_EQUALS(msgA.data[2], 0x11);
	TEST_ASSERT_EQUALS(msgA.data[3], 0x22);
	TEST_ASSERT_EQUALS(msgA.data[4], 0xaa);
	TEST_ASSERT_EQUALS(msgA.data[5], 0xff);
	TEST_ASSERT_EQUALS(msgA.data[6], 0xee);
	TEST_ASSERT_EQUALS(msgA.getLength(), 7);
	TEST_ASSERT_FALSE(msgA.isExtended());

	modm::can::Message msgB(0x432, 1, 0xab, true);
	TEST_ASSERT_EQUALS(msgB.data[0], 0xab);
	TEST_ASSERT_EQUALS(msgB.getLength(), 1);
	TEST_ASSERT_TRUE(msgB.isExtended());
}
