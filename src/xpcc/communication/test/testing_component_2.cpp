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
 * $Id: testing_component_2.cpp 607 2011-09-13 19:51:03Z dergraaf $
 */
// ----------------------------------------------------------------------------

#include "testing_component_2.hpp"

TestingComponent2::TestingComponent2(xpcc::Dispatcher *communication,
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
