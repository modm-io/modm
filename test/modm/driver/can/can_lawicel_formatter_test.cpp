/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2010, Thorsten Lajewski
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2015, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <cstring>

#include "../can_lawicel_formatter.hpp"
#include "can_lawicel_formatter_test.hpp"

void
CanLawicelFormatterTest::testIdentifierToStringExtended()
{
	modm::can::Message msg(0x75395165, 0);

	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}
	
	TEST_ASSERT_TRUE(modm::CanLawicelFormatter::convertToString(msg, buffer));
	
	TEST_ASSERT_EQUALS(std::strlen(buffer), 10U);
	/*              \0-terminatation of string >-+
	 *                                           |
	 *                           data length >-+ |
	 *                                         | |
	 *                                 |--id--|v v       */
	TEST_ASSERT_EQUALS_ARRAY(buffer, "T753951650\0", 11);
}

void
CanLawicelFormatterTest::testIdentifierToStringStandard()
{
	modm::can::Message msg(0x123, 0);
	msg.flags.extended = false;
	
	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}
	
	TEST_ASSERT_TRUE(modm::CanLawicelFormatter::convertToString(msg, buffer));
	
	TEST_ASSERT_EQUALS(std::strlen(buffer), 5U);
	TEST_ASSERT_EQUALS_ARRAY(buffer, "t1230\0", 6);
}

void
CanLawicelFormatterTest::testMessageToStringStandard()
{
	modm::can::Message msg(0x123, 4);
	msg.flags.extended = false;
	msg.data[0] = 0x44;
	msg.data[1] = 0xff;
	msg.data[2] = 0x1A;
	msg.data[3] = 0x12;
	
	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}
	
	TEST_ASSERT_TRUE(modm::CanLawicelFormatter::convertToString(msg, buffer));
	
	TEST_ASSERT_EQUALS(std::strlen(buffer), 13U);
	/*                 \0-terminatation of string >-+
	 *                                              |
	 *                      data length >-+         |
	 *                                    |         |
	 *                                 id v|-data-| v       */
	TEST_ASSERT_EQUALS_ARRAY(buffer, "t123444FF1A12\0", 14);
}

void
CanLawicelFormatterTest::testMessageToStringExtended()
{
	modm::can::Message msg(0x123, 4);
	msg.flags.extended = true;
	msg.data[0] = 0x44;
	msg.data[1] = 0xff;
	msg.data[2] = 0x1A;
	msg.data[3] = 0x12;
	
	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}
	
	TEST_ASSERT_TRUE(modm::CanLawicelFormatter::convertToString(msg, buffer));
	
	TEST_ASSERT_EQUALS(std::strlen(buffer), 18U);
	TEST_ASSERT_EQUALS_ARRAY(buffer, "T00000123444FF1A12\0", 19);
}

void
CanLawicelFormatterTest::testStringToMessage()
{
	const char *input = "T000016108F8FF00002394883D";
	modm::can::Message output;
	
	TEST_ASSERT_TRUE(modm::CanLawicelFormatter::convertToCanMessage(input, output));

	TEST_ASSERT_EQUALS(output.identifier, 0x00001610U);
	TEST_ASSERT_EQUALS(output.length, 8U);
	TEST_ASSERT_EQUALS(output.flags.extended, true);
	TEST_ASSERT_EQUALS(output.flags.rtr, false);
	
	TEST_ASSERT_EQUALS(output.data[0], 0xf8);
	TEST_ASSERT_EQUALS(output.data[1], 0xff);
	TEST_ASSERT_EQUALS(output.data[2], 0x00);
	TEST_ASSERT_EQUALS(output.data[3], 0x00);
	TEST_ASSERT_EQUALS(output.data[4], 0x23);
	TEST_ASSERT_EQUALS(output.data[5], 0x94);
	TEST_ASSERT_EQUALS(output.data[6], 0x88);
	TEST_ASSERT_EQUALS(output.data[7], 0x3d);
}

void
CanLawicelFormatterTest::testRoundtripMessage()
{
	const char *string = "T000016108F8FF00002394883D";
	modm::can::Message message;

	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}

	TEST_ASSERT_TRUE(modm::CanLawicelFormatter::convertToCanMessage(string, message));
	TEST_ASSERT_TRUE(modm::CanLawicelFormatter::convertToString(message, buffer));

	/** check for 0-termination */
	TEST_ASSERT_EQUALS_ARRAY(string, buffer, sizeof(string) + 1);
}



void
CanLawicelFormatterTest::testRoudtripString()
{
	modm::can::Message msg(0x123, 4);
	msg.flags.extended = true;
	msg.data[0] = 0x44;
	msg.data[1] = 0xff;
	msg.data[2] = 0x1A;
	msg.data[3] = 0x12;

	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}

	TEST_ASSERT_TRUE(modm::CanLawicelFormatter::convertToString(msg, buffer));

	modm::can::Message myMsg;
	TEST_ASSERT_TRUE(modm::CanLawicelFormatter::convertToCanMessage(buffer, myMsg));

	TEST_ASSERT_EQUALS(msg.identifier, myMsg.identifier);
	TEST_ASSERT_EQUALS(msg.length, myMsg.length);
	TEST_ASSERT_EQUALS(msg.flags.extended, myMsg.flags.extended);
	TEST_ASSERT_EQUALS(msg.flags.rtr, myMsg.flags.rtr);
	TEST_ASSERT_EQUALS_ARRAY(msg.data, myMsg.data, 4U);
}

void
CanLawicelFormatterTest::testInvalidInput()
{
	const auto& toCanMessage = modm::CanLawicelFormatter::convertToCanMessage;
	modm::can::Message message;

	// id too high only 11 bits supported
	TEST_ASSERT_FALSE(toCanMessage("tfff0", message));
	// one octet missing
	TEST_ASSERT_FALSE(toCanMessage("t0ff300ff4", message));
	// invalid character in payload
	TEST_ASSERT_FALSE(toCanMessage("t0ff30RMf4.", message));
	// invalid character in id
	TEST_ASSERT_FALSE(toCanMessage("t0f.3000000", message));
}
