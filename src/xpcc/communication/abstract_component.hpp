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

#ifndef	XPCC__ABSTRACT_COMPONENT_HPP
#define	XPCC__ABSTRACT_COMPONENT_HPP

#include <stdint.h>
#include "dispatcher.hpp"
#include "communicatable.hpp"
#include "communicator.hpp"

namespace xpcc
{
	/**
	 * \brief	Abstract base class for a component
	 * 
	 * This class give to components an interface to xpcc communication, but
	 * effectively it forwards all requests to the internal created Communicator,
	 * which is bound to this AbstractComponent.
	 *
	 * It is a good convention to give to a component which extends
	 * xpcc::AbstractComponent an update() method which is to be called
	 * during the main loop.
	 *
	 * The programmer is always encouraged to bundle special functionality in
	 * separate classes called Tasks. These Tasks will usually not be
	 * complete components but want send messages in the name of a parent
	 * component. Use xpcc::CommunicatableTask for such type of Tasks.
	 *
	 * \see		xpcc::Task
	 * \see		xpcc::Communicatable
	 *
	 * \ingroup	communication
	 */
	class AbstractComponent : public Communicatable
	{
		friend class CommunicatableTask;
	public:
		/**
		 * \brief	Constructor
		 * 
		 * \param	ownIdentifier	Identifier of the component, must be unique
		 * 							within the network.
		 * \param	communication	Communication class use to send messages
		 */
		AbstractComponent(const uint8_t ownIdentifier,
				Dispatcher *communication);
		
#ifdef __DOXYGEN__
		void
		update();
#endif


	protected:
		inline xpcc::Communicator *
		getCommunicator();

		inline void
		callAction(uint8_t receiver, uint8_t actionIdentifier);
		
		inline void
		callAction(uint8_t receiver, uint8_t actionIdentifier, ResponseCallback& responseCallback);
		
		template<typename T>
		inline void
		callAction(uint8_t receiver, uint8_t actionIdentifier, const T& data);
		
		template<typename T>
		inline void
		callAction(uint8_t receiver, uint8_t actionIdentifier, const T& data, ResponseCallback& responseCallback);
		
		
		inline void
		publishEvent(uint8_t eventIdentifier);
		
		template<typename T>
		inline void
		publishEvent(uint8_t eventIdentifier, const T& data);
		
		
		// [proposition -> dergraaf]: Make these methods only available in the correct
		// circumstances (action call). Perhaps move them methods to the ResponseHandle
		// class?
		inline void
		sendResponse(const ResponseHandle& handle);
		
		template<typename T>
		inline void
		sendResponse(const ResponseHandle& handle, const T& data);
		
		template<typename T>
		inline void
		sendNegativeResponse(const ResponseHandle& handle, const T& data);
		
		inline void
		sendNegativeResponse(const ResponseHandle& handle);
		
	private:
		Communicator communicator;
	};
}

// ---------------------------------------------------------------------
// IMPLEMENTATION
// ---------------------------------------------------------------------
#include "abstract_component_impl.hpp"

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
void
xpcc::AbstractComponent::sendResponse(const ResponseHandle& handle)
{
	this->communicator.sendResponse(handle);
}

void
xpcc::AbstractComponent::sendNegativeResponse(const ResponseHandle& handle)
{
	this->communicator.sendNegativeResponse(handle);
}

#endif // XPCC__ABSTRACT_COMPONENT_HPP
