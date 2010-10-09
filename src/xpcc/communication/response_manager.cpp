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

#include "dispatcher.hpp"
#include "response_manager.hpp"

xpcc::ResponseManager::ResponseManager(BackendInterface *backend,
		Postman* postman) :
	backend(backend),
	postman(postman),
	dummyFirst(Header()),
	first(&dummyFirst),
	last(&dummyFirst)
{
}

bool
xpcc::ResponseManager::Entry::headerFits(const Header& header) const
{
	return ((header.source == this->header.destination) &&
			(header.destination	== this->header.source) &&
			(header.packetIdentifier == this->header.packetIdentifier));
}

// ----------------------------------------------------------------------------
void
xpcc::ResponseManager::append(Entry *next)
{
	this->last->next = next;
	
	// find tail of next
	while (next->next != 0) {
		next = next->next;
	}
	
	this->last = next;
}

void
xpcc::ResponseManager::insertAfter(Entry *fix, Entry *next)
{
	Entry *tmp = fix->next;
	fix->next = next;
	
	while (next->next)// find tail of next
		next = next->next;
	
	if (tmp) {
		next->next = tmp;
	}
	else
	{// fix was the last element, so last has to be updated
		last = next;
	}
}

xpcc::ResponseManager::Entry *
xpcc::ResponseManager::removeNext(Entry *entry)
{
	Entry *next = entry->next;
	if (next)
	{
		entry->next = next->next;
		
		if (next == last) {
			last = entry;
		}
		// next->next = 0; // is null already
		else {
			next->next = 0;
		}
	}
	
	return next;
}

void
xpcc::ResponseManager::handlePacket(const Header& header,
		const SmartPointer& payload)
{
	Entry *prev = first;// first is always a dummy entry
	while (Entry *entry = prev->next)
	{
		if (entry->type == Entry::DEFAULT)
		{
			// waiting for ack, no response can be handled
			EntryDefault *defaultEntry =
					reinterpret_cast<EntryDefault *>(entry);
			if (defaultEntry->headerFits(header))
			{
				// entry e->next has to be removed
				this->removeNext(prev);
				delete defaultEntry;
				
				return;
			}
		}
		else if (entry->type == Entry::CALLBACK)
		{
			EntryWithCallback *callbackEntry = 
					reinterpret_cast<EntryWithCallback *>(entry);
			
			if (callbackEntry->headerFits(header))
			{
				// entry actual has to be marked acknowledged if acknowleded
				// request
				if (header.type == Header::REQUEST)
				{
					// Must be an acknowledge otherwise there is an error in
					// communication, cause no requests can be handled here
					if (header.isAcknowledge)
					{
						// make sure no requests passed here
						callbackEntry->state = Entry::WAIT_FOR_RESPONSE;
					}
				}
				else
				{
					// response or negative response
					if (!header.isAcknowledge) {
						callbackEntry->responseCallback.call(header, payload);
					}
					// else cannot happen, since responses with callbacks are
					// not possible
					
					this->removeNext(prev);
					delete callbackEntry;
				}
				return;
			}
		}
		
		prev = entry;
	}
}

xpcc::ResponseManager::Entry *
xpcc::ResponseManager::deleteEntry(Entry *entry, Entry *prev)
{
	while (prev->next != entry) {
		prev = prev->next;
	}
	
	this->removeNext(prev);
	delete entry;
	
	return prev;
}

xpcc::ResponseManager::Entry *
xpcc::ResponseManager::sendMessageToInnerComponent(Entry *entry, Entry *prev)
{
	// to one component on board inner component
	// send message also out, so it is possible to log
	// communication externally
	backend->sendPacket(entry->header, entry->payload);
	
	if (entry->header.type == Header::REQUEST)
	{
		postman->deliverPacket(entry->header,
				entry->payload);
		// TODO handle postman errors?
		
		if (entry->type == Entry::CALLBACK)
		{
			// TODO timer for RESPOMSES not handeled yet
			entry->state = Entry::WAIT_FOR_RESPONSE;
			entry->time.restart(responseTimeout);
			prev = entry;
		}
		else {
			prev = deleteEntry(entry, prev);
		}
	}
	else
	{// (neg)response
		// remove actual = e->next
		// find one waiting request with callback and handle response
		// responses are insertet at front, requests at end, 
		// so search only after e (e->next is the first candidate)
		//		handle found request and delete it
		// delete actual
	
		this->removeNext(prev);
		
		Entry *tmp = prev;
		while (Entry *s = tmp->next)
		{
			if (s->header.type == Header::REQUEST
					&& s->state != Entry::TRANSMISSION_PENDING // must be WAIT_FOR_RESPONSE
					&& s->headerFits(entry->header))
			{
				
				this->removeNext(tmp);
				
				if (s->type == Entry::CALLBACK)
				{
					((EntryWithCallback*) s)->responseCallback.call(
							entry->header, entry->payload);
				}
	
				delete s;
				break;
			}
			tmp = s;
		}
		delete entry;
	}
	
	return prev;
}

void
xpcc::ResponseManager::handleWaitingMessages()
{
	// first is always a dummy entry
	Entry *prev = first;
	while (Entry *entry = prev->next)
	{
		if (entry->state == Entry::TRANSMISSION_PENDING)
		{
			if (entry->header.destination == 0)
			{
				// event
				postman->deliverPacket(entry->header, entry->payload);
				backend->sendPacket(entry->header, entry->payload);
				
				prev = deleteEntry(entry, prev);
				continue;
			}
			else
			{
				// action or response
				if (postman->isComponentAvaliable(entry->header))
				{
					prev = sendMessageToInnerComponent(entry, prev);
				}
				else
				{
					// destination not on board, message has to be sent
					// out to the backend
					backend->sendPacket(entry->header, entry->payload);
					
					entry->state = Entry::WAIT_FOR_ACK;
					entry->time.restart(acknowledgeTimeout);
					
					prev = entry;
				}
			}
		}
		else if (entry->state == Entry::WAIT_FOR_ACK)
		{
			if (entry->time.isExpired())
			{
				if (entry->tries >= 2)
				{
					// TODO do sth to notify the user
					prev = deleteEntry(entry, prev);
					continue;
				}
				else
				{
					backend->sendPacket(entry->header, entry->payload);
					
					entry->tries++;
					entry->time.restart(acknowledgeTimeout);
				}
			}
			
			prev = entry;
		}
		else 
		{
			// WAIT_FOR_RESPONSE
			// Responses stay in the queue for ever if no response ever
			// comes. This may have to be changed.
			prev = entry;
		}
	}
}

void
xpcc::ResponseManager::addEvent(const Header& header)
{
	SmartPointer p;
	this->addActionCall(header, p);
}

void
xpcc::ResponseManager::addEvent(const Header& header,
		SmartPointer& smartPayload)
{
	Entry *e = new EntryDefault(header, smartPayload);
	e->state = Entry::TRANSMISSION_PENDING;
	
	append(e);
}

void
xpcc::ResponseManager::addResponse(const Header& header)
{
	SmartPointer p;
	this->addResponse(header, p);
}

void
xpcc::ResponseManager::addResponse(const Header& header,
		SmartPointer& smartPayload)
{
	Entry *e = new EntryDefault(header, smartPayload);
	e->state = Entry::TRANSMISSION_PENDING;
	
	this->insertAfter(first, e);
	// it makes response more important, than requests
	// it prevents intern loops. Since it is possible to give a response while 
	// an action is handled and call an action while response is handled
	// one component calling one action on another component on same board
	// handling its response in the same callback-function would cause a loop
	// if responses and actions both are appended at the tail of the list.
	// but now responses are handled in reverse order that's not good
	// what to do? a separator between responses and requests possible?

}

void
xpcc::ResponseManager::addActionCall(const Header& header)
{
	SmartPointer p;
	this->addActionCall(header, p);
}

void
xpcc::ResponseManager::addActionCall(const Header& header,
		SmartPointer& smartPayload)
{
	Entry *e = new EntryDefault(header, smartPayload);
	e->state = Entry::TRANSMISSION_PENDING;
	
	this->append(e);
}

void
xpcc::ResponseManager::addActionCall(const Header& header,
		SmartPointer& smartPayload, Callback& responseCallback)
{
	Entry *e = new EntryWithCallback(header, smartPayload, responseCallback);
	e->state = Entry::TRANSMISSION_PENDING;
	
	append(e);
}

void
xpcc::ResponseManager::addActionCall(const Header& header,
		Callback& responseCallback)
{
	Entry *e = new EntryWithCallback(header, responseCallback);
	e->state = Entry::TRANSMISSION_PENDING;
	
	append(e);
}

