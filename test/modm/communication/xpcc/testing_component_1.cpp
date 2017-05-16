/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "testing_component_1.hpp"

TestingComponent1::TestingComponent1(xpcc::Dispatcher &communication,
		Timeline *timeline) :
	xpcc::AbstractComponent(1, communication),
	timeline(timeline),
	isDelayedResponseActive(false)
{
}

void
TestingComponent1::update()
{
	if (isDelayedResponseActive) {
		this->sendResponse(delayedResponseHandle);
	}
}


// ----------------------------------------------------------------------------
void
TestingComponent1::actionNoParameter(const xpcc::ResponseHandle& handle)
{
	timeline->events.append(
			Timeline::Event(Timeline::ACTION, 1, 0x10, handle.getDestination()));
}

void
TestingComponent1::actionUint16(const xpcc::ResponseHandle& handle,
		const uint16_t *parameter)
{
	timeline->events.append(
			Timeline::Event(Timeline::ACTION, 1, 0x11, handle.getDestination(), parameter));
}

void
TestingComponent1::actionDirectResponse(const xpcc::ResponseHandle& handle)
{
	timeline->events.append(
			Timeline::Event(Timeline::ACTION, 1, 0x12, handle.getDestination()));
	
	this->sendResponse(handle);
}

void
TestingComponent1::actionDelayedResponse(const xpcc::ResponseHandle& handle)
{
	timeline->events.append(
			Timeline::Event(Timeline::ACTION, 1, 0x13, handle.getDestination()));
	
	this->delayedResponseHandle = handle;
	this->isDelayedResponseActive = true;
}

void
TestingComponent1::actionUint16CallAction(const xpcc::ResponseHandle& handle,
			const uint16_t *parameter)
{
	timeline->events.append(
			Timeline::Event(Timeline::ACTION, 1, 0x14, handle.getDestination(), parameter));
	
	this->callAction(2, 0x11, *parameter);
}

// ----------------------------------------------------------------------------
void
TestingComponent1::eventNoParameter(const xpcc::Header& header)
{
	timeline->events.append(
			Timeline::Event(Timeline::EVENT, 2, 0x20, header.source));
}

void
TestingComponent1::eventUint32(const xpcc::Header& header,
		const uint32_t *parameter)
{
	timeline->events.append(
			Timeline::Event(Timeline::EVENT, 1, 0x21, header.source, parameter));
}
