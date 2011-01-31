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

#ifndef	XPCC__COMMUNICATOR_HPP
#define	XPCC__COMMUNICATOR_HPP

#include <stdint.h>
#include "dispatcher.hpp"
#include "communicatable.hpp"

namespace xpcc
{
	/**
	 * \brief	A adapter class which can be obtained from a AbstractComponent.
	 * Use it to enable communication for a group of classes. The component
	 * id will be taken from AbstractComponent.
	 * 
	 * This class is just a forwarder to the Dispatcher like AbstractComponent
	 * it also does.
	 *
	 * \ingroup	communication
	 */
	class Communicator : public Communicatable
	{
	public:
		/**
		 * \brief	Constructor
		 * 
		 * \param	ownIdentifier	Identifier of the component, must be unique
		 * 							within the network.
		 * \param	communication	Communication class use to send messages
		 */
		Communicator(const uint8_t ownIdentifier,
				Dispatcher *dispatcher);
		
		void
		callAction(uint8_t receiver, uint8_t actionIdentifier);
		
		void
		callAction(uint8_t receiver, uint8_t actionIdentifier, ResponseCallback& responseCallback);
		
		template<typename T>
		void
		callAction(uint8_t receiver, uint8_t actionIdentifier, const T& data);
		
		template<typename T>
		void
		callAction(uint8_t receiver, uint8_t actionIdentifier, const T& data, ResponseCallback& responseCallback);
		
		
		void
		publishEvent(uint8_t eventIdentifier);
		
		template<typename T>
		void
		publishEvent(uint8_t eventIdentifier, const T& data);
		
		
		// [proposition -> dergraaf]: Make these methods only available in the correct
		// circumstances (action call). Perhaps move them methods to the ResponseHandle
		// class?
		void
		sendResponse(const ResponseHandle& handle);
		
		template<typename T>
		void
		sendResponse(const ResponseHandle& handle, const T& data);
		
		template<typename T>
		void
		sendNegativeResponse(const ResponseHandle& handle, const T& data);
		
		void
		sendNegativeResponse(const ResponseHandle& handle);
		
	private:
		const uint8_t ownIdentifier;
		Dispatcher * const dispatcher;
	};
}

#include "communicator_impl.hpp"

#endif // XPCC__COMMUNICATOR_HPP
