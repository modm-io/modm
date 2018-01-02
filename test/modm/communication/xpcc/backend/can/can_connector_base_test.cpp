/*
 * Copyright (c) 2010, 2017 Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/communication/xpcc/backend/can/connector.hpp>
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
