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

#include "testing_component_1.hpp"

TestingComponent1::TestingComponent1(xpcc::Dispatcher *communication,
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
