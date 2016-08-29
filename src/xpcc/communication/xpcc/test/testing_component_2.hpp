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

#ifndef	TESTING_COMPONENT_2_HPP
#define	TESTING_COMPONENT_2_HPP

#include <xpcc/communication.hpp>

#include "timeline.hpp"

class TestingComponent2 : public xpcc::AbstractComponent
{
public:
	TestingComponent2(xpcc::Dispatcher &communication, Timeline *timeline);
	
	virtual void
	update();
	
	// expose protected methods
	using xpcc::AbstractComponent::callAction;
	using xpcc::AbstractComponent::publishEvent;
	
	// Action id: 0x10
	void
	actionNoParameter(const xpcc::ResponseHandle& handle);
	
	// Action id: 0x11
	void
	actionUint16(const xpcc::ResponseHandle& handle,
			const uint16_t *parameter);
	
	// Event id: 0x20
	void
	eventNoParameter(const xpcc::Header& header);
	
	
	// id: 0x30
	void
	responseNoParameter(const xpcc::Header& header);
	
	// id: 0x31
	void
	responseUint16(const xpcc::Header& header, const uint16_t *parameter);
	
	// id: 0x32
	void
	responseCallAction(const xpcc::Header& header);
	
private:
	Timeline *timeline;
};

#endif
