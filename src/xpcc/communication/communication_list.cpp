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
 * $Id: postman.hpp 77 2009-10-15 18:34:29Z thundernail $
 */
// ----------------------------------------------------------------------------


#include "communication_list.hpp"

xpcc::CommunicationList::CommunicationList():
first(new CommunicationListEntry(CommunicationListEntry::EMPTY_ENTRY, Header())), last(0){

}

xpcc::CommunicationListEntry::CommunicationListEntry(uint8_t typeInfo, Header header):
next(0),header(header),typeInfo(typeInfo){

}

xpcc::CommunicationListEntry *
xpcc::CommunicationListEntry::getNext()
{
	return next;
}

void
xpcc::CommunicationList::append(CommunicationListEntry *next)
{
	last->next = next;
	
	while(next->next)// find tail of next
		next = next->next;

	last = next;
}

void
xpcc::CommunicationList::insertAfter(CommunicationListEntry *fix, CommunicationListEntry *next)
{
	CommunicationListEntry *tmp = fix->next;
	fix->next = next;
	
	while(next->next)// find tail of next
		next = next->next;
	
	if(tmp){
		next->next = tmp;
	}
	else{// tmp was last
		last = next;
	}
}

xpcc::CommunicationListEntry *
xpcc::CommunicationList::removeNext(CommunicationListEntry *fix)
{
	CommunicationListEntry *tmp = fix->next;
	if(tmp){
		fix->next = tmp->next;
		
		if (tmp == last)
			last = fix;
			// tmp->next = 0; // is is null allready
		else
			tmp->next = 0;
	}
	return tmp;
}

xpcc::CommunicationListEntryWithPayload::CommunicationListEntryWithPayload(Header& header, SmartPayload& smartPayload):
CommunicationListEntry(PAYLOAD, header),smartPayload(smartPayload){
}

xpcc::CommunicationListEntryWithPayloadAndCallback::CommunicationListEntryWithPayloadAndCallback(Header& header, SmartPayload& smartPayload, ResponseCallback& responseCallback):
CommunicationListEntry(PAYLOAD_CALLBACK, header),smartPayload(smartPayload),responseCallback(responseCallback){

}
	
xpcc::CommunicationListEntryWithCallback::CommunicationListEntryWithCallback(Header& header, ResponseCallback& responseCallback):
CommunicationListEntry(CALLBACK, header),responseCallback(responseCallback){
}

void
xpcc::CommunicationList::addActionCall(Header& header, SmartPayload& smartPayload){
	CommunicationListEntryWithPayload* e = new CommunicationListEntryWithPayload(header, smartPayload);
	e->state = CommunicationListEntry::WAIT_FOR_ACK;
	
	this->append(e);
}
		
void
xpcc::CommunicationList::handlePacket(const BackendInterface * const backend){
	const Header& header = backend->getPacketHeader();
	CommunicationListEntry *e = first;// first is always a dummy entry
	while(e->next){// !!! be carefull!! what happends if more than one entriy fits to the header and they are not eaqual?
		switch(e->next->typeInfo){
			case CommunicationListEntry::PAYLOAD:// waiting for ack, no response can be handled
			case CommunicationListEntry::EMPTY_ENTRY:// waiting for ack, no response can be handled
				if(((CommunicationListEntryWithPayload*)e->next)->headerFits(header)){
					// entry e->next has to be removed
					
					e = this->removeNext(e);
					delete e;
					
					return;
				}
				break;
			case CommunicationListEntry::PAYLOAD_CALLBACK:
				if(((CommunicationListEntryWithPayloadAndCallback*)e->next)->headerFits(header)){
					// entry e->next has to be replaced if acknowleded request

					switch(header.type){
						case Header::REQUEST:// must be ack
							if(header.isAcknowledge){// make shure no requests passed here
								e = this->removeNext(e);
								CommunicationListEntryWithCallback *c = new CommunicationListEntryWithCallback(e->header, ((CommunicationListEntryWithPayloadAndCallback*)e)->responseCallback);
								c->state = CommunicationListEntry::ACK_RECEIVED;
								append(c);
								delete e;
							}
							// else there is en error in communication, cause no requests can be handled here
							break;
						default: // response or negative response
							handleResponseOfNextOfEWithCallback<CommunicationListEntryWithPayloadAndCallback>(backend, e);
							break;
					}
					
					return;
				}
				break;
			case CommunicationListEntry::CALLBACK:// waiting for response
				if(((CommunicationListEntryWithCallback*)e->next)->headerFits(header)){
					// entry e->next has to be marked acknowledged if acknowleded request
					switch(header.type){
						case Header::REQUEST:// must be ack
							if(header.isAcknowledge){// make shure no requests passed here
								((CommunicationListEntryWithCallback*)e->next)->state = CommunicationListEntry::ACK_RECEIVED;
							}
							// else there is en error in communication, cause no requests can be handled here
							break;
						default: // response or negative response
							handleResponseOfNextOfEWithCallback<CommunicationListEntryWithCallback>(backend, e);
							break;
					}
					
					return;
				}
				break;
		}
	}
	
	// if here than no handling was possible
}

template<typename C>
void
xpcc::CommunicationList::handleResponseOfNextOfEWithCallback(const BackendInterface * const backend, CommunicationListEntry *e){
	const Header& header = backend->getPacketHeader();
	
	// entry e->next has to be removed
	// handle (neg_)response if (neg_)response, is not acknowledge
	
	// here is no handling of requests, is not checked also
	C *next = (C *)this->removeNext(e);
	if(!header.isAcknowledge){
		switch(header.type){
			case Header::RESPONSE:
				// todo handle response of e
				next->responseCallback.object=0;// delete this line
				break;
			case Header::NEGATIVE_RESPONSE:
				// todo handle negative response of e
				next->responseCallback.object=0;// delete this line
				break;
			default:
				break;
		}
	}
	delete e;
}

