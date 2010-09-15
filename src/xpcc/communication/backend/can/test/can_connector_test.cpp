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

#include "can_connector_test.hpp"

// ----------------------------------------------------------------------------
void
CanConnectorTest::checkShortMessage(const xpcc::can::Message& message) const
{
	TEST_ASSERT_EQUALS(message.identifier, normalIdentifier);
	TEST_ASSERT_EQUALS(message.length, sizeof(shortPayload));
	TEST_ASSERT_EQUALS_ARRAY(message.data, shortPayload, sizeof(shortPayload));
}

uint8_t
CanConnectorTest::getPayloadLength(uint8_t offset) const
{
    uint8_t payloadLength = sizeof (fragmentedPayload) - offset;
    if (payloadLength > 6) {
        payloadLength = 6;
    }
    return payloadLength;
}

void
CanConnectorTest::checkFragmentedMessage(const xpcc::can::Message& message,
		uint8_t fragmentId) const
{
	uint8_t offset = fragmentId * 6;
	uint8_t payloadLength = getPayloadLength(offset);
    
	TEST_ASSERT_EQUALS(message.identifier, fragmentedIdentifier);
	TEST_ASSERT_EQUALS(message.length, payloadLength + 2);
	
	TEST_ASSERT_EQUALS(message.data[0], fragmentId | messageCounter);
	TEST_ASSERT_EQUALS(message.data[1], sizeof(fragmentedPayload));
	TEST_ASSERT_EQUALS_ARRAY(&message.data[2],
			&fragmentedPayload[offset], payloadLength);
}

void
CanConnectorTest::createMessage(xpcc::can::Message& message,
		uint8_t fragmentId) const
{
	uint8_t offset = fragmentId * 6;
	uint8_t payloadLength = getPayloadLength(offset);
	
	message.identifier = fragmentedIdentifier;
	message.length = payloadLength + 2;
	
	message.data[0] = fragmentId | messageCounter;
	message.data[1] = sizeof(fragmentedPayload);
	memcpy(	&message.data[2],
			&fragmentedPayload[offset],
			payloadLength );
}

// ----------------------------------------------------------------------------
void
CanConnectorTest::setUp()
{
	this->driver = new FakeCanDriver();
	this->connector = new TestingCanConnector(this->driver);
}

void
CanConnectorTest::tearDown()
{
	delete this->connector;
	delete this->driver;
}

// ----------------------------------------------------------------------------
CanConnectorTest::CanConnectorTest() :
	xpccHeader(xpcc::Header::REQUEST, false, 0x12, 0x34, 0x56),
	normalIdentifier(0x00123456),
	fragmentedIdentifier(0x01123456)
{
	for (uint8_t i = 0; i < sizeof(shortPayload); ++i) {
		shortPayload[i] = i;
	}
	
	for (uint8_t i = 0; i < sizeof(fragmentedPayload); ++i) {
		fragmentedPayload[i] = i * 2;
	}
}

// ----------------------------------------------------------------------------
void
CanConnectorTest::testSendShortMessageDirect()
{
	driver->sendSlots = 1;
	
	xpcc::SmartPointer payload(&shortPayload);
	connector->sendPacket(xpccHeader, payload);
	
	// short messages might be send directly without any call to update
	TEST_ASSERT_EQUALS(driver->sendList.getSize(), 1U);
	checkShortMessage(driver->sendList.getFront());
}

void
CanConnectorTest::testSendShortMessage()
{
	xpcc::SmartPointer payload(&shortPayload);
	connector->sendPacket(xpccHeader, payload);
	
	TEST_ASSERT_EQUALS(driver->sendList.getSize(), 0U);
	
	driver->sendSlots = 1;
	connector->update();
	
	TEST_ASSERT_EQUALS(driver->sendList.getSize(), 1U);
	checkShortMessage(driver->sendList.getFront());
}

void
CanConnectorTest::testSendFragmentedMessage()
{
	driver->sendSlots = 2;
	this->messageCounter = connector->messageCounter = 0x30;
	
	xpcc::SmartPointer payload(&fragmentedPayload);
	connector->sendPacket(xpccHeader, payload);
	
	// fragmented messages aren't send directly but queued immediately
	TEST_ASSERT_EQUALS(driver->sendList.getSize(), 0U);
	
	// with two send slots two message should be send
	connector->update();
	TEST_ASSERT_EQUALS(driver->sendList.getSize(), 1U);
	connector->update();
	TEST_ASSERT_EQUALS(driver->sendList.getSize(), 2U);
	
	checkFragmentedMessage(driver->sendList.getFront(), 0);
	driver->sendList.removeFront();
	
	checkFragmentedMessage(driver->sendList.getFront(), 1);
	driver->sendList.removeFront();
	
	driver->sendSlots = 1;
	connector->update();
	connector->update();
	TEST_ASSERT_EQUALS(driver->sendList.getSize(), 1U);
	
	checkFragmentedMessage(driver->sendList.getFront(), 2);
	driver->sendList.removeFront();
	
	TEST_ASSERT_EQUALS(connector->messageCounter, 0x40);
}

void
CanConnectorTest::testReceiveShortMessage()
{
	TEST_ASSERT_FALSE(connector->isPacketAvailable());
	
	// create a new can message
	xpcc::can::Message message(normalIdentifier, 8);
	memcpy(&message.data, shortPayload, 8);
	
	driver->receiveList.append(message);
	
	// let it be received by the connector
	connector->update();
	
	TEST_ASSERT_TRUE(connector->isPacketAvailable());
	
	TEST_ASSERT_EQUALS(connector->getPacketHeader(), xpccHeader);
	TEST_ASSERT_EQUALS(connector->getPacketPayload().getSize(), 8U);
	TEST_ASSERT_EQUALS_ARRAY(
			connector->getPacketPayload().getPointer(),
			shortPayload,
			8);
	connector->dropPacket();
	
	TEST_ASSERT_FALSE(connector->isPacketAvailable());
}

void
CanConnectorTest::testReceiveFragmentedMessage()
{
	TEST_ASSERT_FALSE(connector->isPacketAvailable());
	
	this->messageCounter = 0x50;
	xpcc::can::Message message;
	
	createMessage(message, 0);
	driver->receiveList.append(message);
	
	createMessage(message, 1);
	driver->receiveList.append(message);
	
	connector->update();
	TEST_ASSERT_FALSE(connector->isPacketAvailable());
	
	// Send the matching fragment with a wrong message counter.
	// This should be treated as a new message and not append to the peding one.
	this->messageCounter = 0xf0;
	createMessage(message, 2);
	driver->receiveList.append(message);
	
	connector->update();
	TEST_ASSERT_FALSE(connector->isPacketAvailable());
	
	// Now the last fragment with the correct counter
	this->messageCounter = 0x50;
	createMessage(message, 2);
	driver->receiveList.append(message);
	
	connector->update();
	TEST_ASSERT_TRUE(connector->isPacketAvailable());
	
	// check if the message is correct
	TEST_ASSERT_EQUALS(connector->getPacketHeader(), xpccHeader);
	TEST_ASSERT_EQUALS(connector->getPacketPayload().getSize(), sizeof(fragmentedPayload));
	TEST_ASSERT_EQUALS_ARRAY(
			connector->getPacketPayload().getPointer(),
			fragmentedPayload,
			sizeof(fragmentedPayload));
	connector->dropPacket();
	
	TEST_ASSERT_FALSE(connector->isPacketAvailable());
	
	// send the first two fragments for the message with the different
	// message counter
	this->messageCounter = 0xf0;
	createMessage(message, 1);
	driver->receiveList.append(message);
	
	connector->update();
	TEST_ASSERT_FALSE(connector->isPacketAvailable());
	
	createMessage(message, 0);
	driver->receiveList.append(message);
	
	connector->update();
	TEST_ASSERT_TRUE(connector->isPacketAvailable());
	
	// check if the message is correct
	TEST_ASSERT_EQUALS(connector->getPacketHeader(), xpccHeader);
	TEST_ASSERT_EQUALS(connector->getPacketPayload().getSize(), sizeof(fragmentedPayload));
	TEST_ASSERT_EQUALS_ARRAY(
			connector->getPacketPayload().getPointer(),
			fragmentedPayload,
			sizeof(fragmentedPayload));
	connector->dropPacket();
	
	TEST_ASSERT_FALSE(connector->isPacketAvailable());
}
