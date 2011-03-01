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

#include <xpcc/communication/action_callback.hpp>

#include "action_callback_test.hpp"

class TestComponent : public xpcc::AbstractComponent
{
public:
	TestComponent() :
		xpcc::AbstractComponent(12, 0),
		methodWithParameterCalled(false),
		methodWithoutParameterCalled(false),
		parameter(0)
	{
	}
	
	void
	methodWithoutParameter(const xpcc::ResponseHandle&)
	{
		methodWithoutParameterCalled = true;
	}
	
	void
	methodWithParameter(const xpcc::ResponseHandle&, const uint32_t *parameter)
	{
		methodWithParameterCalled = true;
		this->parameter = *parameter;
	}
	
	bool methodWithParameterCalled;
	bool methodWithoutParameterCalled;
	uint32_t parameter;
};

// ----------------------------------------------------------------------------
void
ActionCallbackTest::testNoParameter()
{
	TestComponent component;
	xpcc::ActionCallback callback(&component, &TestComponent::methodWithoutParameter);
	
	xpcc::Header header;
	xpcc::SmartPointer payload;
	
	callback.call(xpcc::ResponseHandle(header), payload);
	
	TEST_ASSERT_TRUE(component.methodWithoutParameterCalled);
}

// ----------------------------------------------------------------------------
void
ActionCallbackTest::testParameter()
{
	TestComponent component;
	xpcc::ActionCallback callback(&component, &TestComponent::methodWithParameter);
	
	xpcc::Header header;
	uint32_t parameter = 0x12345678;
	xpcc::SmartPointer payload(&parameter);
	
	callback.call(xpcc::ResponseHandle(header), payload);
	
	TEST_ASSERT_TRUE(component.methodWithParameterCalled);
	TEST_ASSERT_EQUALS(component.parameter, parameter);
}
