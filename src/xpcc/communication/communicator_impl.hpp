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
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__COMMUNICATOR_HPP
	#error	"Don't include this file directly, use 'communicator.hpp' instead"
#endif


// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Communicator::callAction(uint8_t receiver,
		uint8_t actionIdentifier, const T& data)
{
	Header header(Header::REQUEST, false,
			receiver,
			this->ownIdentifier,
			actionIdentifier);
	
	SmartPointer payload(&data);
	
	this->dispatcher->addMessage(header, payload);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Communicator::callAction(uint8_t receiver, uint8_t actionIdentifier,
		const T& data, ResponseCallback& responseCallback)
{
	Header header(Header::REQUEST, false,
			receiver,
			this->ownIdentifier,
			actionIdentifier);
	
	SmartPointer payload(&data);
	
	this->dispatcher->addMessage(header, payload, responseCallback);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Communicator::publishEvent(uint8_t eventIdentifier, const T& data)
{
	Header header(Header::REQUEST, false,
			0,
			this->ownIdentifier,
			eventIdentifier);
	
	SmartPointer payload(&data);	// no metadata is sent with Events
	this->dispatcher->addMessage(header, payload);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Communicator::sendResponse(const ResponseHandle& handle, const T& data)
{
	Header header(Header::RESPONSE, false,
			handle.destination,
			this->ownIdentifier,
			handle.packetIdentifier);
	
	SmartPointer payload(&data);
	this->dispatcher->addResponse(header, payload);
}

template<typename T>
void
xpcc::Communicator::sendNegativeResponse(const ResponseHandle& handle, const T& data)
{
	Header header(Header::NEGATIVE_RESPONSE, false,
			handle.destination,
			this->ownIdentifier,
			handle.packetIdentifier);
	
	SmartPointer payload(&data);
	this->dispatcher->addResponse(header, payload);
}
