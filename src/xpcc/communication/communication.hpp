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
 * $Id: communication.hpp 76 2009-10-14 23:29:28Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_COMMUNICATION_HPP
#define	XPCC_COMMUNICATION_HPP

#include "backend/backend_interface.hpp"
#include "abstract_component.hpp"
#include "postman.hpp"
#include "communication_list.hpp"

//template<typename T>
//const T*
//getData() {
//	return static_cast<const T*>(data);
//};

//Position& p = getData<Position>();



//template<typename T>
//bool
//getData(T& data) {
//	if (size != sizeof(T)) {
//		return false;
//	}
//	else {
//		memcpy((void *) T, data, sizeof(T));
//		return true;
//	}
//};

//Position p;
//getData(p);
//if(getData(p));


namespace xpcc
{
	typedef Header ResponseHandle;
	
	/// @ingroup	communication
	class Communication
	{
	public:
		Communication(BackendInterface *backend,
				Postman* postman);
		
		~Communication();
				
		void
		update();
		
		
		uint8_t
		getCurrentComponent() const;
		
		void
		setCurrentComponent(uint8_t id);
		
		template<typename T>
		void
		callAction(uint8_t receiver, uint8_t actionIdentifier, const T& data);
		
		template<typename T>
		void
		callAction(uint8_t receiver, uint8_t actionIdentifier, const T& data, ResponseCallback& responseCallback);
		
		template<typename T>
		void
		sendResponse(const ResponseHandle& handle, const T& data);
		
		template<typename T>
		void
		sendNegativeResponse(const ResponseHandle& handle, const T& data);
		
		template<typename T>
		void
		publishEvent(uint8_t eventIdentifier, const T& data);
		
	private:
		BackendInterface * const backend;
		Postman * const postman;
		uint8_t currentComponent;
		CommunicationList responseManager;
		
		void
		waitForAcknowledge(const Header &header, const SmartPayload& payload);
		
		
	};
}

#endif // XPCC_COMMUNICATION_HPP
