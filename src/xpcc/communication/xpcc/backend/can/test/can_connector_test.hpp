/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef CAN_CONNECTOR_TEST_HPP
#define CAN_CONNECTOR_TEST_HPP

#include <unittest/testsuite.hpp>

#include "testing_can_connector.hpp"

class CanConnectorTest : public unittest::TestSuite
{
	void
	checkShortMessage(const xpcc::can::Message& message) const;
	
	uint8_t
    getPayloadLength(uint8_t offset) const;
	
	void
	checkFragmentedMessage(const xpcc::can::Message& message,
			uint8_t fragmentId) const;
	
	void
	createMessage(xpcc::can::Message& message,
			uint8_t fragmentId) const;
public:
    virtual void
    setUp();
    
    virtual void
    tearDown();
    
public:
    CanConnectorTest();
    
    void
    testSendShortMessageDirect();
    
    void
    testSendShortMessage();
    
    void
    testSendFragmentedMessage();
    
    void
    testReceiveShortMessage();
    
    void
    testReceiveFragmentedMessage();
    
private:
	TestingCanConnector *connector;
	FakeCanDriver *driver;
	
	xpcc::Header xpccHeader;
	uint32_t normalIdentifier;
	uint32_t fragmentedIdentifier;
	uint8_t messageCounter;
	
	uint8_t shortPayload[8];
	uint8_t fragmentedPayload[14];
};

#endif
