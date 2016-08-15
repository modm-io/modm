// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "can_message_test.hpp"

void
CanMessageTest::testEqualOperator()
{
	// Construct two equal messages

	xpcc::can::Message msgA(0x12);
	msgA.setLength(2);
	msgA.data[0] = 0x82;
	msgA.data[1] = 0x22;

	xpcc::can::Message msgB(0x12);
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
