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
 *
 * $Id: canusb_formater_test.cpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#include <cstring>

#include "../canusb_formater.hpp"
#include "canusb_formater_test.hpp"

void
CanusbFormaterTest::testIdentifierToStringExtended()
{
	xpcc::can::Message msg(0x12345678, 0);
	char buffer[128];
	
	xpcc::CanUsbFormater::convertToString(msg, buffer);
	
	TEST_ASSERT_EQUALS_ARRAY(buffer, "T12345678", 9);
}

void
CanusbFormaterTest::testIdentifierToStringStandard()
{
	xpcc::can::Message msg(0x123, 0);
	msg.flags.extended = false;
	
	char buffer[128];
	
	xpcc::CanUsbFormater::convertToString(msg, buffer);
	
	TEST_ASSERT_EQUALS_ARRAY(buffer, "t123", 4);
}

void
CanusbFormaterTest::testMessageToStringStandard()
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
	
	xpcc::CanUsbFormater::convertToString(msg, buffer);
	
	TEST_ASSERT_EQUALS(std::strlen(buffer), 13U);
	TEST_ASSERT_EQUALS_ARRAY(buffer, "t123444FF1A12", 13);
}

void
CanusbFormaterTest::testMessageToStringExtended()
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
	
	xpcc::CanUsbFormater::convertToString(msg, buffer);
	
	TEST_ASSERT_EQUALS(std::strlen(buffer), 18U);
	TEST_ASSERT_EQUALS_ARRAY(buffer, "T00000123444FF1A12", 18);
}

void
CanusbFormaterTest::testStringToMessage()
{
	const char *input = "T000016108F8FF00002394883D";
	xpcc::can::Message output;
	
	TEST_ASSERT_TRUE(xpcc::CanUsbFormater::convertToCanMessage(input, output));
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
