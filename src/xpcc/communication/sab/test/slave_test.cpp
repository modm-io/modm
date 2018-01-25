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

#include "slave_test.hpp"

namespace
{
	class TestingObject : public xpcc::sab::Callable
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
		emptyFunction(xpcc::sab::Response& /*response*/)
		{
			calledFunction = EMPTY;
		}
		
		void
		responseFunction(xpcc::sab::Response& response)
		{
			calledFunction = RESPONSE;
			
			uint16_t value = 0xabcd;
			response.send(value);
		}
		
		void
		errorFunction(xpcc::sab::Response& response)
		{
			calledFunction = ERROR;
			
			response.error();
		}
		
		void
		parameterFunction(xpcc::sab::Response& response, const uint16_t* parameter)
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

	// FIXME: FLASH_STORAGE for AVR
	const xpcc::sab::Action actionList[] =
	{
		SAB__ACTION(0x01, testingObject, TestingObject::emptyFunction, 0),
		SAB__ACTION(0x02, testingObject, TestingObject::responseFunction, 0),
		SAB__ACTION(0x03, testingObject, TestingObject::errorFunction, 0),
		SAB__ACTION(0x04, testingObject, TestingObject::parameterFunction, 2),
	};
}

// ----------------------------------------------------------------------------
void
SlaveTest::setUp()
{
	FakeIODevice::reset();
	testingObject.reset();
	
	slave = new TestingSlave(0x3f,
					xpcc::accessor::asFlash(actionList),
					sizeof(actionList) / sizeof(xpcc::sab::Action));
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
	TestingInterface::sendMessage(0x3f, xpcc::sab::REQUEST, 0x01);
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
	TestingInterface::sendMessage(0x3f, xpcc::sab::REQUEST, 0x02);
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
	TestingInterface::sendMessage(0x3f, xpcc::sab::REQUEST, 0x03);
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
	
	TestingInterface::sendMessage(0x3f, xpcc::sab::REQUEST, 0x04, value);
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
	
	TestingInterface::sendMessage(0x3f, xpcc::sab::REQUEST, 0x04, value);
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
	TestingInterface::sendMessage(0x3f, xpcc::sab::REQUEST, 0x05);
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
