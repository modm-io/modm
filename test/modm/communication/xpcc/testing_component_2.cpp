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

#include "testing_component_2.hpp"

TestingComponent2::TestingComponent2(xpcc::Dispatcher &communication,
		Timeline *timeline) :
	xpcc::AbstractComponent(2, communication),
	timeline(timeline)
{
}

void
TestingComponent2::update()
{
}


// ----------------------------------------------------------------------------
void
TestingComponent2::actionNoParameter(const xpcc::ResponseHandle& handle)
{
	timeline->events.append(
			Timeline::Event(Timeline::ACTION, 2, 0x10, handle.getDestination()));
}

void
TestingComponent2::actionUint16(const xpcc::ResponseHandle& handle,
		const uint16_t *parameter)
{
	timeline->events.append(
			Timeline::Event(Timeline::ACTION, 2, 0x11, handle.getDestination(),
					parameter));
}

// ----------------------------------------------------------------------------
void
TestingComponent2::eventNoParameter(const xpcc::Header& header)
{
	timeline->events.append(
			Timeline::Event(Timeline::EVENT, 2, 0x10, header.source));
}


// ----------------------------------------------------------------------------
void
TestingComponent2::responseNoParameter(const xpcc::Header& header)
{
	timeline->events.append(
			Timeline::Event(Timeline::RESPONSE, 2, 0x30, header.source));
}

void
TestingComponent2::responseUint16(const xpcc::Header& header,
		const uint16_t *parameter)
{
	timeline->events.append(
			Timeline::Event(Timeline::RESPONSE, 2, 0x31, header.source, parameter));
}

void
TestingComponent2::responseCallAction(const xpcc::Header& header)
{
	timeline->events.append(
			Timeline::Event(Timeline::RESPONSE, 2, 0x32, header.source));
	
	xpcc::ResponseCallback callback(this, &TestingComponent2::responseNoParameter);
	this->callAction(1, 0x12, callback);
}
