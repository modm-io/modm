/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, 2017, Fabian Greif
 * Copyright (c) 2012-2013, 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/communication/sab/interface.hpp>

#include "fake_io_device.hpp"
#include "interface_test.hpp"

typedef modm::sab::Interface<FakeIODevice> TestingInterface;

void
InterfaceTest::setUp()
{
	// set up everything
	TestingInterface::initialize();
	FakeIODevice::reset();
}

// ----------------------------------------------------------------------------
void
InterfaceTest::testSendRequest()
{
	TestingInterface interface;
	
	uint32_t data = 0xdeadbeef;
	interface.sendMessage(0x12, modm::sab::REQUEST, 0x34, data);
	
	uint8_t testMessage[9] = {
		0x54, 4, 0x12, 0x34, 0xef, 0xbe, 0xad, 0xde, 238
	};
	
	TEST_ASSERT_EQUALS(FakeIODevice::bytesSend, 9);
	TEST_ASSERT_EQUALS_ARRAY(FakeIODevice::sendBuffer, testMessage, 9);
}

void
InterfaceTest::testSendAck()
{
	TestingInterface interface;
	
	uint16_t data = 0xbaaa;
	interface.sendMessage(0x3f, modm::sab::ACK, 0x56, data);
	
	uint8_t testMessage[7] = {
		0x54, 2, 0x3f | 0xc0, 0x56, 0xaa, 0xba, 135
	};
	
	TEST_ASSERT_EQUALS(FakeIODevice::bytesSend, 7);
	TEST_ASSERT_EQUALS_ARRAY(FakeIODevice::sendBuffer, testMessage, 7);
}

#ifdef MODM_COMPILER_MSVC
	#pragma pack(push, 1)
	struct Data
	{
		int16_t a;
		uint8_t b;
		uint32_t c;
	};
	#pragma pack(pop)
#else
	struct Data
	{
		int16_t a;
		uint8_t b;
		uint32_t c;
	} __attribute__((packed));
#endif

void
InterfaceTest::testSendNack()
{
	TestingInterface interface;
	
	Data data =
	{
		-1,
		12,
		0x12345678
	};
	
	interface.sendMessage(0x06, modm::sab::NACK, 0x78, data);
	
	uint8_t testMessage[12] = {
		0x54, 7, 0x06 | 0x80, 0x78, 0xff, 0xff, 12, 0x78, 0x56, 0x34, 0x12, 193
	};
	
	TEST_ASSERT_EQUALS(FakeIODevice::bytesSend, 12);
	TEST_ASSERT_EQUALS_ARRAY(FakeIODevice::sendBuffer, testMessage, 12);
}

// ----------------------------------------------------------------------------
void
InterfaceTest::testReceive()
{
	TestingInterface interface;
	
	// write a new message into the FakeIODevice
	uint32_t data = 0xdeadbeef;
	interface.sendMessage(0x12, modm::sab::REQUEST, 0x34, data);
	
	FakeIODevice::moveSendToReceiveBuffer();
	
	// ... and try to receive it again
	interface.update();
	
	TEST_ASSERT_TRUE(interface.isMessageAvailable());
	
	TEST_ASSERT_FALSE(interface.isResponse());
	TEST_ASSERT_FALSE(interface.isAcknowledge());
	TEST_ASSERT_EQUALS(interface.getAddress(), 0x12);
	TEST_ASSERT_EQUALS(interface.getCommand(), 0x34);
	TEST_ASSERT_EQUALS(interface.getPayloadLength(), 4);
	TEST_ASSERT_EQUALS_ARRAY(
			interface.getPayload(),
			reinterpret_cast<uint8_t *>(&data),
			4);
}

// ----------------------------------------------------------------------------
void
InterfaceTest::testReceiveAck()
{
	TestingInterface interface;
	
	// write a new message into the FakeIODevice
	uint32_t data = 0xdeadbeef;
	interface.sendMessage(0x12, modm::sab::ACK, 0x34, data);
	
	FakeIODevice::moveSendToReceiveBuffer();
	
	// ... and try to receive it again
	interface.update();
	
	TEST_ASSERT_TRUE(interface.isMessageAvailable());
	
	TEST_ASSERT_TRUE(interface.isResponse());
	TEST_ASSERT_TRUE(interface.isAcknowledge());
	TEST_ASSERT_EQUALS(interface.getAddress(), 0x12);
	TEST_ASSERT_EQUALS(interface.getCommand(), 0x34);
	TEST_ASSERT_EQUALS(interface.getPayloadLength(), 4);
	TEST_ASSERT_EQUALS_ARRAY(
			interface.getPayload(),
			reinterpret_cast<uint8_t *>(&data),
			4);
}

// ----------------------------------------------------------------------------
void
InterfaceTest::testReceiveNack()
{
	TestingInterface interface;
	
	// write a new message into the FakeIODevice
	uint32_t data = 0xdeadbeef;
	interface.sendMessage(0x12, modm::sab::NACK, 0x34, data);
	
	FakeIODevice::moveSendToReceiveBuffer();
	
	// ... and try to receive it again
	interface.update();
	
	TEST_ASSERT_TRUE(interface.isMessageAvailable());
	
	TEST_ASSERT_TRUE(interface.isResponse());
	TEST_ASSERT_FALSE(interface.isAcknowledge());
	TEST_ASSERT_EQUALS(interface.getAddress(), 0x12);
	TEST_ASSERT_EQUALS(interface.getCommand(), 0x34);
	TEST_ASSERT_EQUALS(interface.getPayloadLength(), 4);
	TEST_ASSERT_EQUALS_ARRAY(
			interface.getPayload(),
			reinterpret_cast<uint8_t *>(&data),
			4);
}
