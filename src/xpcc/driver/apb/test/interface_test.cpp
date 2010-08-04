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
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <xpcc/driver/apb/interface.hpp>
#include "fake_io_device.hpp"
#include "interface_test.hpp"

typedef xpcc::apb::Interface<FakeIODevice> TestingInterface;

void
InterfaceTest::setUp()
{
	// set up everything
	TestingInterface::initialize();
	FakeIODevice::reset();
}

// ----------------------------------------------------------------------------
void
InterfaceTest::testSend()
{
	TestingInterface interface;
	
	uint32_t data = 0xdeadbeef;
	interface.sendMessage(0x12, false, 0x34, data);
	
	uint8_t testMessage[9] = {
		0x54, 4, 0x12, 0x34, 0xef, 0xbe, 0xad, 0xde, 238
	};
	
	TEST_ASSERT_EQUALS(FakeIODevice::bytesSend, 9);
	TEST_ASSERT_EQUALS_ARRAY(FakeIODevice::sendBuffer, testMessage, 9);
}

void
InterfaceTest::testReceive()
{
	TestingInterface interface;
	
	// write a new message into the FakeIODevice
	uint32_t data = 0xdeadbeef;
	interface.sendMessage(0x12, false, 0x34, data);
	
	FakeIODevice::moveSendToReceiveBuffer();
	
	// ... and try to receive it again
	interface.update();
	
	TEST_ASSERT_TRUE(interface.isMessageAvailable());
	
	TEST_ASSERT_FALSE(interface.isAcknowledge());
	TEST_ASSERT_EQUALS(interface.getAddress(), 0x12);
	TEST_ASSERT_EQUALS(interface.getCommand(), 0x34);
	TEST_ASSERT_EQUALS(interface.getPayloadLength(), 4);
	TEST_ASSERT_EQUALS_ARRAY(
			interface.getPayload(),
			reinterpret_cast<uint8_t *>(&data),
			4);
}
