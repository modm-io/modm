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

#include <xpcc/debug/logger/logger.hpp>

#include "communication_list.hpp"

// set the Loglevel
#undef  XPCC_LOG_LEVEL
#define XPCC_LOG_LEVEL xpcc::log::DEBUG


xpcc::communicationList::List::List(xpcc::Communication *communication) :
	communication(communication), dummyFirst(Header()),
	first(&dummyFirst), last(&dummyFirst)
{
}

xpcc::communicationList::Entry::Entry(uint8_t typeInfo, const Header& header, SmartPointer &payload) :
	typeInfo(typeInfo),
	next(0),
	header(header),
	payload(payload),
	time(),
	tries(0)
{
}

xpcc::communicationList::Entry::Entry(uint8_t typeInfo, const Header& header) :
	typeInfo(typeInfo), 
	next(0), 
	header(header), 
	payload(SmartPointer()),
	time(),
	tries(0)
{
}

const xpcc::communicationList::Entry *
xpcc::communicationList::Entry::getNext() const
{
	return next;
}

bool
xpcc::communicationList::Entry::headerFits(const Header& header) const
{
	return header.source == this->header.destination && 
			header.destination == this->header.source &&
			header.packetIdentifier == this->header.packetIdentifier;
}

xpcc::communicationList::EntryDefault::EntryDefault(const Header& header):
Entry(DEFAULT, header){
}

xpcc::communicationList::EntryDefault::EntryDefault(const Header& header, SmartPointer& payload):
Entry(DEFAULT, header, payload){
}

xpcc::communicationList::EntryWithCallback::EntryWithCallback(const Header& header, SmartPointer& payload, Callback& responseCallback):
Entry(CALLBACK, header, payload),
responseCallback(responseCallback){
}

xpcc::communicationList::EntryWithCallback::EntryWithCallback(const Header& header, Callback& responseCallback):
Entry(CALLBACK, header),
responseCallback(responseCallback){
}

void
xpcc::communicationList::List::append(Entry *next)
{
	this->last->next = next;
	
	while( next->next != 0 )// find tail of next
		next = next->next;

	this->last = next;
}

void
xpcc::communicationList::List::insertAfter(Entry *fix, Entry *next)
{
	Entry *tmp = fix->next;
	fix->next = next;
	
	while(next->next)// find tail of next
		next = next->next;
	
	if(tmp){
		next->next = tmp;
	}
	else{// fix was the last element, so last has to be uptaded
		last = next;
	}
}

xpcc::communicationList::Entry *
xpcc::communicationList::List::removeNext(Entry *fix)
{
	Entry *tmp = fix->next;
	if(tmp){
		fix->next = tmp->next;
		
		if (tmp == last)
			last = fix;
			// tmp->next = 0; // is null allready
		else
			tmp->next = 0;
	}
	return tmp;
}

void
xpcc::communicationList::List::handlePacket(const Header& header, const SmartPointer& payload)
{
	Entry *e = first;// first is always a dummy entry
	while(Entry *actual = e->next){// !!! be carefull!! it must not happen, that more than one entry fits to the header
		switch(actual->typeInfo){
			case Entry::DEFAULT:// waiting for ack, no response can be handled
				if(!((EntryDefault*)actual)->headerFits(header)){
					e = actual;
					break;
				}
				// entry e->next has to be removed
				
//				XPCC_LOG_INFO << "Communication Remove default" << xpcc::endl;
				this->removeNext(e);
				delete actual;
				
				return;
			case Entry::CALLBACK:// waiting for response
				if(!((EntryWithCallback*)actual)->headerFits(header)){
					e = actual;
					break;
				}
				// entry actual has to be marked acknowledged if acknowleded request
				if(header.type == Header::REQUEST){
					// must be ack
					if(header.isAcknowledge){// make shure no requests passed here
						((EntryWithCallback*)actual)->state = Entry::WAIT_FOR_RESPONSE;
					}
					// else there is en error in communication, cause no requests can be handled here
				}
				else{// response or negative response
					
					// entry actual = e->next has to be removed, and deleted
					// handle (neg_)response if (neg_)response, is not acknowledge
					
					EntryWithCallback *c = (EntryWithCallback *)this->removeNext(e);
					
					if(!header.isAcknowledge){
						c->responseCallback.call(header, payload);
					}
					// else cannot happen, since responses with callbacks are not possible
					
					delete c;
				}
				return;
		}
	}
	
	// if here than no handling was possible
}

void
xpcc::communicationList::List::handleWaitingMessages(Postman &postman, BackendInterface &backend){
	Entry *e = first;// first is always a dummy entry
	uint8_t listSize = 0;
	while(Entry *actual = e->next){
		listSize++;
		
		switch(actual->state){
			case Entry::WANT_TO_BE_SENT:
//				XPCC_LOG_INFO << "Communication WTBS" << xpcc::endl;
				if (actual->header.destination == 0){// event
					postman.deliverPacket(actual->header, actual->payload);
					backend.sendPacket(actual->header, actual->payload);
					while( e->next != actual ) {
						e = e->next;
					}
					this->removeNext(e);
					delete actual;
				}
				else{// action or response
					if(postman.isComponentAvaliable(actual->header))
					{// to one component on board inner component
						
						// send message also out, so it is possible to log communication extern
						backend.sendPacket(actual->header, actual->payload);
						
						if (actual->header.type == Header::REQUEST)
						{
							communication->setCurrentComponent(actual->header.destination);
							postman.deliverPacket(actual->header, actual->payload);
							// todo handle postman errors?
							if (actual->typeInfo == Entry::CALLBACK){
								actual->state = Entry::WAIT_FOR_RESPONSE;
								actual->time.restart(100); // TODO timer for RESPOMSES not handeled yet
								e = actual;
							}
							else{
								while( e->next != actual ) {
									e = e->next;
								}
								this->removeNext(e);
								delete actual;
							}
						}
						else {// (neg)response
							// remove actual = e->next
							// find one waiting request with callback and handle response
							// responses are insertet at front, requests at end, 
							// so search only after e (e->next is the first candidate)
							//		handle found request and delete it
							// delete actual
							
							this->removeNext(e);
							
							Entry *tmp = e;
							while (Entry *s = tmp->next) {
								if (s->header.type == Header::REQUEST 
										&& s->state != Entry::WANT_TO_BE_SENT // must be WAIT_FOR_RESPONSE
										&& s->headerFits(actual->header)) {
									
									this->removeNext(tmp);
									
									if(s->typeInfo == Entry::CALLBACK){
										((EntryWithCallback*)s)->responseCallback.call(actual->header, actual->payload);
									}
									
									delete s;
									break;
								}
								tmp = s;
							}
							delete actual;
						}
					}
					else {// destination not on board, message has to be sent out to backend
						backend.sendPacket(actual->header, actual->payload);
						actual->state = Entry::WAIT_FOR_ACK;
						actual->time.restart(100);
						e = actual;
					}
				}
				
				break;
			case Entry::WAIT_FOR_ACK:
			{
//				XPCC_LOG_INFO << "Communication WTFA" << xpcc::endl;
				if (actual->time.isExpired()){
					if (actual->tries >= 2){
						this->removeNext(e);
						// TODO do sth to notify the user
						delete actual;
					}
					else {
						actual->tries++;
						backend.sendPacket(actual->header, actual->payload);
						actual->time.restart(100);
						e = actual;
					}
				}
				else{
					e = actual;
				}
				break;
			}
			default: // WAIT_FOR_RESPONSE think about if it has to be deleted, actually responses stay in the queue for ever if no response comes
				e = actual;
				break; // no action
		}
	
	}
//	if (listSize > 0)
//		XPCC_LOG_INFO << "Communication = " << listSize << xpcc::endl;
}

void
xpcc::communicationList::List::addEvent(const Header& header)
{
	SmartPointer p;
	this->addActionCall(header, p);
}

void
xpcc::communicationList::List::addEvent(const Header& header, SmartPointer& smartPayload)
{
	Entry *e = new EntryDefault(header, smartPayload);
	e->state = Entry::WANT_TO_BE_SENT;

	append(e);
}

void
xpcc::communicationList::List::addResponse(const Header& header)
{
	SmartPointer p;
	this->addResponse(header, p);
}

void
xpcc::communicationList::List::addResponse(const Header& header, SmartPointer& smartPayload)
{
	Entry *e = new EntryDefault(header, smartPayload);
	e->state = Entry::WANT_TO_BE_SENT;

	this->insertAfter(first, e);
	// it makes response more important, than requests
	// it prevents intern loops. Since it is possible to give a response while 
	// an action is handled and call an action while response is handeled
	// one component calling one action on another component on same board
	// handling its response in the same callbackfunction would cause a loop
	// if responses and actions both are apended at the tail of the list.
	// but now responses are handeled in reverse order that's not good
	// what to do? a separator between responses and requests possible?
	
}

void
xpcc::communicationList::List::addActionCall(const Header& header){
	SmartPointer p;
	this->addActionCall(header, p);
}

void
xpcc::communicationList::List::addActionCall(const Header& header, SmartPointer& smartPayload){
	Entry *e = new EntryDefault(header, smartPayload);
	e->state = Entry::WANT_TO_BE_SENT;
	
	this->append(e);
}

void
xpcc::communicationList::List::addActionCall(const Header& header, SmartPointer& smartPayload, Callback& responseCallback){
	Entry *e = new EntryWithCallback(header, smartPayload, responseCallback);
	e->state = Entry::WANT_TO_BE_SENT;

	append(e);
}

void
xpcc::communicationList::List::addActionCall(const Header& header, Callback& responseCallback){
	Entry *e = new EntryWithCallback(header, responseCallback);
	e->state = Entry::WANT_TO_BE_SENT;
	
	append(e);
}

