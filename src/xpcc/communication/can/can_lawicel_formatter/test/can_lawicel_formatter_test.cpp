// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#include <cstring>

#include "../can_lawicel_formatter.hpp"
#include "can_lawicel_formatter_test.hpp"

void
CanLawicelFormatterTest::testIdentifierToStringExtended()
{
	xpcc::can::Message msg(0x75395165, 0);

	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}
	
	TEST_ASSERT_TRUE(xpcc::CanLawicelFormatter::convertToString(msg, buffer));
	
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
	xpcc::can::Message msg(0x123, 0);
	msg.flags.extended = false;
	
	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}
	
	TEST_ASSERT_TRUE(xpcc::CanLawicelFormatter::convertToString(msg, buffer));
	
	TEST_ASSERT_EQUALS(std::strlen(buffer), 5U);
	TEST_ASSERT_EQUALS_ARRAY(buffer, "t1230\0", 6);
}

void
CanLawicelFormatterTest::testMessageToStringStandard()
{
	xpcc::can::Message msg(0x123, 4);
	msg.flags.extended = false;
	msg.data[0] = 0x44;
	msg.data[1] = 0xff;
	msg.data[2] = 0x1A;
	msg.data[3] = 0x12;
	
	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}
	
	TEST_ASSERT_TRUE(xpcc::CanLawicelFormatter::convertToString(msg, buffer));
	
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
	xpcc::can::Message msg(0x123, 4);
	msg.flags.extended = true;
	msg.data[0] = 0x44;
	msg.data[1] = 0xff;
	msg.data[2] = 0x1A;
	msg.data[3] = 0x12;
	
	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}
	
	TEST_ASSERT_TRUE(xpcc::CanLawicelFormatter::convertToString(msg, buffer));
	
	TEST_ASSERT_EQUALS(std::strlen(buffer), 18U);
	TEST_ASSERT_EQUALS_ARRAY(buffer, "T00000123444FF1A12\0", 19);
}

void
CanLawicelFormatterTest::testStringToMessage()
{
	const char *input = "T000016108F8FF00002394883D";
	xpcc::can::Message output;
	
	TEST_ASSERT_TRUE(xpcc::CanLawicelFormatter::convertToCanMessage(input, output));

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
	xpcc::can::Message message;

	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}

	TEST_ASSERT_TRUE(xpcc::CanLawicelFormatter::convertToCanMessage(string, message));
	TEST_ASSERT_TRUE(xpcc::CanLawicelFormatter::convertToString(message, buffer));

	/** check for 0-termination */
	TEST_ASSERT_EQUALS_ARRAY(string, buffer, sizeof(string) + 1);
}



void
CanLawicelFormatterTest::testRoudtripString()
{
	xpcc::can::Message msg(0x123, 4);
	msg.flags.extended = true;
	msg.data[0] = 0x44;
	msg.data[1] = 0xff;
	msg.data[2] = 0x1A;
	msg.data[3] = 0x12;

	char buffer[128];
	for (int i = 0; i < 128; ++i) {
		buffer[i] = 'a';
	}

	TEST_ASSERT_TRUE(xpcc::CanLawicelFormatter::convertToString(msg, buffer));

	xpcc::can::Message myMsg;
	TEST_ASSERT_TRUE(xpcc::CanLawicelFormatter::convertToCanMessage(buffer, myMsg));

	TEST_ASSERT_EQUALS(msg.identifier, myMsg.identifier);
	TEST_ASSERT_EQUALS(msg.length, myMsg.length);
	TEST_ASSERT_EQUALS(msg.flags.extended, myMsg.flags.extended);
	TEST_ASSERT_EQUALS(msg.flags.rtr, myMsg.flags.rtr);
	TEST_ASSERT_EQUALS_ARRAY(msg.data, myMsg.data, 4U);
}




