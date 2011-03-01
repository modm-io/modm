// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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

#ifndef	XPCC__ABSTRACT_COMPONENT_HPP
	#error	"Don't include this file directly, use 'abstract_component.hpp' instead"
#endif

// ----------------------------------------------------------------------------
xpcc::Communicator *
xpcc::AbstractComponent::getCommunicator()
{
	return &this->communicator;
}

// ----------------------------------------------------------------------------
void
xpcc::AbstractComponent::callAction(uint8_t receiver, uint8_t actionIdentifier)
{
	this->communicator.callAction(receiver, actionIdentifier);
}

void
xpcc::AbstractComponent::callAction(uint8_t receiver, uint8_t actionIdentifier, ResponseCallback& responseCallback)
{
	this->communicator.callAction(receiver, actionIdentifier, responseCallback);
}


// ----------------------------------------------------------------------------
void
xpcc::AbstractComponent::publishEvent(uint8_t eventIdentifier)
{
	this->communicator.publishEvent(eventIdentifier);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::AbstractComponent::callAction(uint8_t receiver,
		uint8_t actionIdentifier, const T& data)
{
	this->communicator.callAction(receiver, actionIdentifier, data);
}

template<typename T>
void
xpcc::AbstractComponent::callAction(uint8_t receiver, uint8_t actionIdentifier,
		const T& data, ResponseCallback& responseCallback)
{
	this->communicator.callAction(receiver, actionIdentifier, data, responseCallback);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::AbstractComponent::publishEvent(uint8_t eventIdentifier, const T& data)
{
	communicator.publishEvent(eventIdentifier, data);
}

// ----------------------------------------------------------------------------
void
xpcc::AbstractComponent::sendResponse(const ResponseHandle& handle)
{
	this->communicator.sendResponse(handle);
}

template<typename T>
void
xpcc::AbstractComponent::sendResponse(const ResponseHandle& handle, const T& data)
{
	this->communicator.sendResponse(handle, data);
}

void
xpcc::AbstractComponent::sendNegativeResponse(const ResponseHandle& handle)
{
	this->communicator.sendNegativeResponse(handle);
}

template<typename T>
void
xpcc::AbstractComponent::sendNegativeResponse(const ResponseHandle& handle, const T& data)
{
	this->communicator.sendNegativeResponse(handle, data);
}
