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
 * $Id: postman.hpp 77 2009-10-15 18:34:29Z thundernail $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_COMMUNICATION_LIST_HPP
#define	XPCC_COMMUNICATION_LIST_HPP


#include "backend/backend_interface.hpp"
#include "response_callback.hpp"

namespace xpcc{

	class CommunicationListEntry{
	
	public:
	
		typedef enum {
			EMPTY_ENTRY,
			PAYLOAD,
			CALLBACK,
			PAYLOAD_CALLBACK,
		} TypeInfo;
		
	
		CommunicationListEntry(uint8_t typeInfo, Header header);
		
		bool
		headerFits(Header header);
		
		CommunicationListEntry *
		getNext();
		
		CommunicationListEntry *next;
		typedef enum {
			WANT_TO_BE_SENT,
			WAIT_FOR_ACK,
			WAIT_FOR_RESPONSE,
			ACK_RECEIVED,
		} State;
		
		// timeout
		Header header;
		State state;
		
		const uint8_t typeInfo;
	};

	class CommunicationListEntryWithPayload:public CommunicationListEntry{
	
	public:
		CommunicationListEntryWithPayload(Header& header, SmartPayload& smartPayload);
		
		SmartPayload smartPayload;
	};

	class CommunicationListEntryWithPayloadAndCallback:public CommunicationListEntry{
	
	public:
		CommunicationListEntryWithPayloadAndCallback(Header& header, SmartPayload& smartPayload, ResponseCallback& callback);

		SmartPayload smartPayload;
		ResponseCallback responseCallback;
	};

	class CommunicationListEntryWithCallback:public CommunicationListEntry{

	public:
		CommunicationListEntryWithCallback(Header& header, ResponseCallback& callback);
		
		ResponseCallback responseCallback;
	};

	class CommunicationList{
	public:
		CommunicationList();
		
		void
		addResponse(Header& header, SmartPayload& smartPayload);
		
		void
		addActionCall(uint8_t id, Header& header);
		
		void
		addActionCall(Header& header, SmartPayload& smartPayload);
		
		void
		addActionCall(Header& header, SmartPayload& smartPayload, ResponseCallback& responseCallback);
		
		void
		addActionCall(Header& header, ResponseCallback& responseCallback);
		
		void
		handlePacket(const BackendInterface * const backend);
		
	private:
		void
		append(CommunicationListEntry *next);
		
		void
		insertAfter(CommunicationListEntry *fix, CommunicationListEntry *next);

		CommunicationListEntry *
		removeNext(CommunicationListEntry *fix);
		

		template<typename C>
		void
		handleResponseOfNextOfEWithCallback(const BackendInterface * const backend, CommunicationListEntry *e);

		CommunicationListEntry *first;
		CommunicationListEntry *last;
		
	};
}

#endif // XPCC_COMMUNICATION_LIST_HPP
