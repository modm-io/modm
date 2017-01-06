/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef DISPATCHER_TEST_HPP
#define DISPATCHER_TEST_HPP

#include <unittest/testsuite.hpp>

#include <modm/communication/xpcc/dispatcher.hpp>

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
