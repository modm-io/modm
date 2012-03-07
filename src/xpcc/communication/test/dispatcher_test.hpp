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
 * $Id: dispatcher_test.hpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef DISPATCHER_TEST_HPP
#define DISPATCHER_TEST_HPP

#include <unittest/testsuite.hpp>

#include <xpcc/communication/dispatcher.hpp>

#include "fake_backend.hpp"
#include "fake_postman.hpp"
#include "timeline.hpp"

/**
 * \brief	Test of the central part of the communication
 * 
 * Following scenario:
 * Component 1 and 2 implemented locally, Component 10 somewhere outside.
 * 
 * 
 */
class DispatcherTest : public unittest::TestSuite
{
public:
	virtual void
	setUp();
	
	virtual void
	tearDown();
	
public:
	/*
	 * Step 1:
	 * Check the reception of unexpected messages from the outside
	 */
	
	// Check REQUEST for a local component, must generate ACK
	void
	testReceiveRequest();
	
	// Check REQUEST for a component unknown to the postman
	void
	testReceiveRequestNoComponent();
	
	// Check RESPONSE for a local component, must generate ACK
	void
	testReceiveResponse();
	
	// Check RESPONSE for a unknown component (isComponentAvailable() == false)
	void
	testReceiveResponseNoComponent();
	
	/*
	 * Step 2:
	 * Check Event transmission and reception
	 */ 
	void
	testEventReception();
	
	void
	testEventTransmission();
	
	/*
	 * Step 3:
	 * Check internal action calls
	 */
	void
	testInternalActionCallNoParameter();
	
	void
	testInternalActionCallUnit16();
	
	void
	testInternalActionCallDirectResponse();
	
	void
	testInternalActionCallDelayedResponse();
	
	// The called method does a action call inside the callback method 
	void
	testInternalActionCallWithRecall();
	
	void
	testInternalActionBackAndForth();
	
	/*
	 * Step 4:
	 * Check retransmission on missing ACK
	 */
	void
	testActionRetransmissionWithAbort();
	
	void
	testActionRetransmission();
	
	void
	testResponseRetransmissionWithAbort();
	
	void
	testResponseRetransmission();
	
private:
	xpcc::Dispatcher *dispatcher;
	FakeBackend *backend;
	FakePostman *postman;
	
	TestingComponent1 *component1;
	TestingComponent2 *component2;
	
	Timeline *timeline;
};

#endif
