/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "slave_test.hpp"

namespace
{
	class TestingObject : public modm::sab::Callable
	{
	public:
		TestingObject() :
			calledFunction(NONE), returnValue(0x12345678), receivedParameter(0)
		{
		}
		
		void
		reset()
		{
			receivedParameter = 0;
			calledFunction = NONE;
		}
		
		
		void
		emptyFunction(modm::sab::Response& /*response*/)
		{
			calledFunction = EMPTY;
		}
		
		void
		responseFunction(modm::sab::Response& response)
		{
			calledFunction = RESPONSE;
			
			uint16_t value = 0xabcd;
			response.send(value);
		}
		
		void
		errorFunction(modm::sab::Response& response)
		{
			calledFunction = ERROR;
			
			response.error();
		}
		
		void
		parameterFunction(modm::sab::Response& response, const uint16_t* parameter)
		{
			calledFunction = PARAMETER;
			
			receivedParameter = *parameter;
			response.send(returnValue);
		}
		
		enum FunctionCode
		{
			NONE,
			EMPTY,
			RESPONSE,
			ERROR,
			PARAMETER,
		};
		
		FunctionCode calledFunction;
		const uint32_t returnValue;
		uint16_t receivedParameter;
	};
	
	TestingObject testingObject;
	
	// FIXME: Use FLASH_STORAGE
	const modm::sab::Action actionList[] =
	{
		SAB_ACTION(0x01, testingObject, TestingObject::emptyFunction, 0),
		SAB_ACTION(0x02, testingObject, TestingObject::responseFunction, 0),
		SAB_ACTION(0x03, testingObject, TestingObject::errorFunction, 0),
		SAB_ACTION(0x04, testingObject, TestingObject::parameterFunction, 2),
	};
}

// ----------------------------------------------------------------------------
void
SlaveTest::setUp()
{
	FakeIODevice::reset();
	testingObject.reset();
	
	slave = new TestingSlave(0x3f,
					modm::accessor::asFlash(actionList),
					sizeof(actionList) / sizeof(modm::sab::Action));
}

void
SlaveTest::tearDown()
{
	delete slave;
}

// ----------------------------------------------------------------------------
void
SlaveTest::testEmptyMethod()
{
	TestingInterface::sendMessage(0x3f, modm::sab::REQUEST, 0x01);
	FakeIODevice::moveSendToReceiveBuffer();
	
	slave->update();
	TEST_ASSERT_EQUALS(testingObject.calledFunction, TestingObject::EMPTY);
	
	// receive message send by slave
	FakeIODevice::moveSendToReceiveBuffer();
	TestingInterface::update();
	
	TEST_ASSERT_TRUE(TestingInterface::isMessageAvailable());
	TEST_ASSERT_TRUE(TestingInterface::isResponse());
	TEST_ASSERT_FALSE(TestingInterface::isAcknowledge());
	TEST_ASSERT_EQUALS(TestingInterface::getAddress(), 0x3f);
	TEST_ASSERT_EQUALS(TestingInterface::getCommand(), 0x01);
	TEST_ASSERT_EQUALS(TestingInterface::getPayloadLength(), 1);
}

void
SlaveTest::testResponseMethod()
{
	TestingInterface::sendMessage(0x3f, modm::sab::REQUEST, 0x02);
	FakeIODevice::moveSendToReceiveBuffer();
	
	slave->update();
	
	TEST_ASSERT_EQUALS(testingObject.calledFunction, TestingObject::RESPONSE);
	
	// receive message send by slave
	FakeIODevice::moveSendToReceiveBuffer();
	TestingInterface::update();
	
	TEST_ASSERT_TRUE(TestingInterface::isMessageAvailable());
	TEST_ASSERT_TRUE(TestingInterface::isResponse());
	TEST_ASSERT_TRUE(TestingInterface::isAcknowledge());
	TEST_ASSERT_EQUALS(TestingInterface::getAddress(), 0x3f);
	TEST_ASSERT_EQUALS(TestingInterface::getCommand(), 0x02);
	TEST_ASSERT_EQUALS(TestingInterface::getPayloadLength(), 2);
	
	const uint16_t value = 0xabcd;
	TEST_ASSERT_EQUALS_ARRAY(TestingInterface::getPayload(),
			reinterpret_cast<const uint8_t *>(&value), 2);
}

void
SlaveTest::testErrorResponse()
{
	TestingInterface::sendMessage(0x3f, modm::sab::REQUEST, 0x03);
	FakeIODevice::moveSendToReceiveBuffer();
	
	slave->update();
	
	TEST_ASSERT_EQUALS(testingObject.calledFunction, TestingObject::ERROR);
	
	// receive message send by slave
	FakeIODevice::moveSendToReceiveBuffer();
	TestingInterface::update();
	
	TEST_ASSERT_TRUE(TestingInterface::isMessageAvailable());
	TEST_ASSERT_TRUE(TestingInterface::isResponse());
	TEST_ASSERT_FALSE(TestingInterface::isAcknowledge());
	TEST_ASSERT_EQUALS(TestingInterface::getAddress(), 0x3f);
	TEST_ASSERT_EQUALS(TestingInterface::getCommand(), 0x03);
	TEST_ASSERT_EQUALS(TestingInterface::getPayloadLength(), 1);
}

void
SlaveTest::testParameterMethod()
{
	uint16_t value = 0x9876;
	
	TestingInterface::sendMessage(0x3f, modm::sab::REQUEST, 0x04, value);
	FakeIODevice::moveSendToReceiveBuffer();
	
	slave->update();
	
	TEST_ASSERT_EQUALS(testingObject.calledFunction, TestingObject::PARAMETER);
	TEST_ASSERT_EQUALS(testingObject.receivedParameter, value);
	
	// receive message send by slave
	FakeIODevice::moveSendToReceiveBuffer();
	TestingInterface::update();
	
	TEST_ASSERT_TRUE(TestingInterface::isMessageAvailable());
	TEST_ASSERT_TRUE(TestingInterface::isResponse());
	TEST_ASSERT_TRUE(TestingInterface::isAcknowledge());
	TEST_ASSERT_EQUALS(TestingInterface::getAddress(), 0x3f);
	TEST_ASSERT_EQUALS(TestingInterface::getCommand(), 0x04);
	TEST_ASSERT_EQUALS(TestingInterface::getPayloadLength(), 4);
	
	TEST_ASSERT_EQUALS_ARRAY(TestingInterface::getPayload(),
			reinterpret_cast<const uint8_t *>(&testingObject.returnValue), 4);
}

void
SlaveTest::testWrongParameterSize()
{
	uint32_t value = 0xbaab;
	
	TestingInterface::sendMessage(0x3f, modm::sab::REQUEST, 0x04, value);
	FakeIODevice::moveSendToReceiveBuffer();
	
	slave->update();
	
	TEST_ASSERT_EQUALS(testingObject.calledFunction, TestingObject::NONE);
	
	// receive message send by slave
	FakeIODevice::moveSendToReceiveBuffer();
	TestingInterface::update();
	
	TEST_ASSERT_TRUE(TestingInterface::isMessageAvailable());
	TEST_ASSERT_TRUE(TestingInterface::isResponse());
	TEST_ASSERT_FALSE(TestingInterface::isAcknowledge());
	TEST_ASSERT_EQUALS(TestingInterface::getAddress(), 0x3f);
	TEST_ASSERT_EQUALS(TestingInterface::getCommand(), 0x04);
	TEST_ASSERT_EQUALS(TestingInterface::getPayloadLength(), 1);
}

void
SlaveTest::testNoMethod()
{
	TestingInterface::sendMessage(0x3f, modm::sab::REQUEST, 0x05);
	FakeIODevice::moveSendToReceiveBuffer();
	
	slave->update();
	
	TEST_ASSERT_EQUALS(testingObject.calledFunction, TestingObject::NONE);
	
	// receive message send by slave
	FakeIODevice::moveSendToReceiveBuffer();
	TestingInterface::update();
	
	TEST_ASSERT_TRUE(TestingInterface::isMessageAvailable());
	
	TEST_ASSERT_TRUE(TestingInterface::isResponse());
	TEST_ASSERT_FALSE(TestingInterface::isAcknowledge());
	TEST_ASSERT_EQUALS(TestingInterface::getAddress(), 0x3f);
	TEST_ASSERT_EQUALS(TestingInterface::getCommand(), 0x05);
	TEST_ASSERT_EQUALS(TestingInterface::getPayloadLength(), 1);
}
