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

#include "../connector.hpp"
#include "can_connector_base_test.hpp"

// ----------------------------------------------------------------------------
void
CanConnectorBaseTest::testConversionToIdentifier()
{
	xpcc::CanConnectorBase connector;
	xpcc::Header header;

	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false), 0x00000000U);
	header = xpcc::Header(xpcc::Header::Type::RESPONSE, false, 0, 0, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false), 0x08000000U);
	header = xpcc::Header(xpcc::Header::Type::NEGATIVE_RESPONSE, false, 0, 0, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false), 0x10000000U);

	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, true),  0x01000000U);

	header = xpcc::Header(xpcc::Header::Type::REQUEST, true, 0, 0, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false), 0x04000000U);

	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0xff, 0, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x00ff0000U);
	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0xcc, 0, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x00cc0000U);
	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0x0a, 0, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x000a0000U);

	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0xff, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x0000ff00U);
	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0xcc, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x0000cc00U);
	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0x0a, 0);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x00000a00U);

	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0, 0xff);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x000000ffU);
	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0, 0xcc);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x000000ccU);
	header = xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0, 0x0a);
	TEST_ASSERT_EQUALS(connector.convertToIdentifier(header, false),  0x0000000aU);
}

void
CanConnectorBaseTest::testConversionToHeader()
{
	xpcc::CanConnectorBase connector;

	uint32_t identifier;
	xpcc::Header header;

	identifier = 0x00000000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0, 0));

	identifier = 0x08000000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::RESPONSE, false, 0, 0, 0));

	identifier = 0x10000000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::NEGATIVE_RESPONSE, false, 0, 0, 0));

	identifier = 0x04000000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, true, 0, 0, 0));

	identifier = 0x01000000;
	TEST_ASSERT_TRUE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0, 0));

	identifier = 0x00ff0000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0xff, 0, 0));
	identifier = 0x00cc0000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0xcc, 0, 0));
	identifier = 0x000a0000;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0x0a, 0, 0));

	identifier = 0x0000ff00;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0xff, 0));
	identifier = 0x0000cc00;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0xcc, 0));
	identifier = 0x00000a00;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0x0a, 0));

	identifier = 0x000000ff;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0, 0xff));
	identifier = 0x000000cc;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0, 0xcc));
	identifier = 0x0000000a;
	TEST_ASSERT_FALSE(connector.convertToHeader(identifier, header));
	TEST_ASSERT_EQUALS(header, xpcc::Header(xpcc::Header::Type::REQUEST, false, 0, 0, 0x0a));
}
