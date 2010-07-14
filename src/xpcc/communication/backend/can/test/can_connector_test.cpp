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

#include <xpcc/communication/backend/can/can_connector.hpp>

#include "can_connector_test.hpp"

// ----------------------------------------------------------------------------
namespace
{
	class FakeCanDriver
	{
	public:
		static inline bool
		isMessageAvailable()
		{
			return true;
		}
		
		static bool
		getMessage(xpcc::Can::Message& message)
		{
			message = FakeCanDriver::message;
			return true;
		}
		
		static bool
		isReadyToSend()
		{
			return true;
		}
		
		static bool
		sendMessage(const xpcc::Can::Message& message)
		{
			FakeCanDriver::message = message;
			return true;
		}
		
		static xpcc::Can::Message message;
	};
	
	xpcc::Can::Message FakeCanDriver::message;
	
	class TestConnector : public xpcc::CanConnector<FakeCanDriver>
	{
	public:
		// make the protected methods public for testing
		using xpcc::CanConnector<FakeCanDriver>::convertToIdentifier;
		using xpcc::CanConnector<FakeCanDriver>::convertToHeader;
	};
}

// ----------------------------------------------------------------------------
void
CanConnectorTest::testConstruction()
{
	TestConnector connector;
}

void
CanConnectorTest::testConversionToIdentifier()
{
	TestConnector connector;
	
	xpcc::Header header;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false), 0x00000000U);
	
	header.type = xpcc::Header::RESPONSE;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false), 0x08000000U);
	
	header.type = xpcc::Header::NEGATIVE_RESPONSE;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false), 0x10000000U);
	
	header.type = xpcc::Header::REQUEST;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false), 0x00000000U);
	
	header.isAcknowledge = true;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false), 0x04000000U);	
	
	header.isAcknowledge = false;
	
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, true),  0x01000000U);
	
	header.destination = 0xff;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x00ff0000U);
	header.destination = 0xcc;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x00cc0000U);
	header.destination = 0x0a;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x000a0000U);
	
	header.destination = 0;
	
	header.source = 0xff;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x0000ff00U);
	header.source = 0xcc;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x0000cc00U);
	header.source = 0x0a;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x00000a00U);
	
	header.source = 0;
	
	header.packetIdentifier = 0xff;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x000000ffU);
	header.packetIdentifier = 0xcc;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x000000ccU);
	header.packetIdentifier = 0x0a;
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x0000000aU);
}

void
CanConnectorTest::testConversionToHeader()
{
	TestConnector connector;
	
	uint32_t identifier;
	xpcc::Header header;
	
	identifier = 0x00000000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0, 0, 0));
	
	identifier = 0x08000000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::RESPONSE, false, 0, 0, 0));
	
	identifier = 0x10000000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::NEGATIVE_RESPONSE, false, 0, 0, 0));
	
	identifier = 0x04000000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, true, 0, 0, 0));
	
	identifier = 0x01000000;
	TEST_ASSERT_TRUE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0, 0, 0));
	
	identifier = 0x00ff0000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0xff, 0, 0));
	identifier = 0x00cc0000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0xcc, 0, 0));
	identifier = 0x000a0000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0x0a, 0, 0));
	
	identifier = 0x0000ff00;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0, 0xff, 0));
	identifier = 0x0000cc00;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0, 0xcc, 0));
	identifier = 0x00000a00;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0, 0x0a, 0));
	
	identifier = 0x000000ff;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0, 0, 0xff));
	identifier = 0x000000cc;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0, 0, 0xcc));
	identifier = 0x0000000a;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::REQUEST, false, 0, 0, 0x0a));
}
