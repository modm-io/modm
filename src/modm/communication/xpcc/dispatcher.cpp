/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009-2011, Georgi Grinshpun
 * Copyright (c) 2012-2013, 2015, 2017-2018, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "dispatcher.hpp"

#include <modm/debug/logger/logger.hpp>
// set the Loglevel
#undef  MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::INFO

xpcc::Dispatcher::Dispatcher(BackendInterface *backend_, Postman* postman_) :
	backend(backend_), postman(postman_)
{
}

// ----------------------------------------------------------------------------
void
xpcc::Dispatcher::update()
{
	this->backend->update();

	//Check if a new packet was received by the backend
	while (this->backend->isPacketAvailable())
	{
		const Header& header = this->backend->getPacketHeader();
		const modm::SmartPointer& payload = this->backend->getPacketPayload();

		if (header.type == Header::Type::REQUEST && !header.isAcknowledge)
		{
			this->handleActionCall(header, payload);
		}
		else
		{
			const bool is_response = this->handlePacket(header, payload);
			if (!header.isAcknowledge && header.destination != 0)
            {
            	if (is_response or postman->isComponentAvailable(header.destination))
                    this->sendAcknowledge(header);
            }
		}

		this->backend->dropPacket();
	}

	// check if there are packets to send
	this->handleWaitingMessages();
}

void
xpcc::Dispatcher::handleActionCall(const Header& header,
		const modm::SmartPointer& payload)
{
	xpcc::Postman::DeliverInfo result = postman->deliverPacket(header, payload);

	if (result == Postman::OK && header.destination != 0)
	{
		// transmit ACK:
		// Message is not an EVENT and the destination is outside
		// this board because internal messages aren't received
		// by the backend
		this->sendAcknowledge(header);
	}

	// TODO Error reporting
}

void
xpcc::Dispatcher::sendAcknowledge(const Header& header)
{
	Header ackHeader(
			header.type, true,
			header.source, header.destination,
			header.packetIdentifier);

	this->backend->sendPacket(ackHeader);
}

bool
xpcc::Dispatcher::Entry::headerFits(const Header& inHeader) const
{
	return ((inHeader.source == this->header.destination) &&
			(inHeader.destination	== this->header.source) &&
			(inHeader.packetIdentifier == this->header.packetIdentifier));
}

bool
xpcc::Dispatcher::handlePacket(const Header& header,
		const modm::SmartPointer& payload)
{
	bool ack = false;
	auto entry = this->entries.begin();
	while(entry != this->entries.end())
	{
		if (entry->type == Entry::Type::Default)
		{
			// waiting for ack, no response can be handled
			if (entry->headerFits(header))
			{
				entry = this->entries.remove(entry);
				return ack;
			}
		}
		else if (entry->type == Entry::Type::Callback)
		{
			if (entry->headerFits(header))
			{
				// entry actual has to be marked acknowledged if acknowleded
				// request
				if (header.type == Header::Type::REQUEST)
				{
					// Must be an acknowledge otherwise there is an error in
					// communication, cause no requests can be handled here
					if (header.isAcknowledge)
					{
						// make sure no requests passed here
						entry->state = Entry::State::WaitForResponse;
					}
				}
				else
				{
					// response or negative response
					if (!header.isAcknowledge) {
						entry->callbackResponse(header, payload);
						ack = true;
					} else {
						// cannot happen, since responses with callbacks are
						// not possible
					}
					entry = this->entries.remove(entry);
				}
				return ack;
			}
		}
		++entry;
	}
	return ack;
}

xpcc::Dispatcher::EntryIterator
xpcc::Dispatcher::sendMessageToInnerComponent(EntryIterator entry)
{
	// to one component on board inner component
	// send message also out, so it is possible to log
	// communication externally
	backend->sendPacket(entry->header, entry->payload);

	if (entry->header.type == Header::Type::REQUEST)
	{
		postman->deliverPacket(entry->header, entry->payload);
		// TODO handle postman errors?

		if (entry->type == Entry::Type::Callback)
		{
			// TODO timer for RESPONSES not handeled yet
			entry->state = Entry::State::WaitForResponse;
			entry->time.restart(responseTimeout);
			return entry;
		}
		else {
			return this->entries.remove(entry);
		}
	}
	else
	{
		// packet is a (NEG)RESPONSE
		//
		// we need to find the coresponding REQUEST and delete it as well
		// as the RESPONSE
		//
		// responses are inserted at front, requests at end,
		// thus we only need to search after the RESPONSE

		auto req = entry;
		for (++req; req != this->entries.end(); ++req)
		{
			if (req->header.type == Header::Type::REQUEST and
			    // must be State::WaitForResponse
			    req->state != Entry::State::TransmissionPending and
			    req->headerFits(entry->header))
			{
				if (req->type == Entry::Type::Callback)
				{
					req->callbackResponse(entry->header, entry->payload);
				}
				this->entries.remove(req);
				break;
			}
		}

		return this->entries.remove(entry);
	}

	return entry;
}

void
xpcc::Dispatcher::handleWaitingMessages()
{
	auto entry = this->entries.begin();
	while(entry != this->entries.end())
	{
		if (entry->state == Entry::State::TransmissionPending)
		{
			if (entry->header.destination == 0)
			{
				// event
				postman->deliverPacket(entry->header, entry->payload);
				backend->sendPacket(entry->header, entry->payload);

				entry = this->entries.remove(entry);
				continue;
			}
			else
			{
				// action or response
				if (postman->isComponentAvailable(entry->header.destination))
				{
					entry = sendMessageToInnerComponent(entry);
					continue;
				}
				else
				{
					// destination not on board, message has to be sent
					// out to the backend
					backend->sendPacket(entry->header, entry->payload);

					entry->state = Entry::State::WaitForACK;
					entry->time.restart(acknowledgeTimeout);

					++entry;
				}
			}
		}
		else if (entry->state == Entry::State::WaitForACK)
		{
			if (entry->time.isExpired())
			{
				if (entry->tries >= 2)
				{
                    Header header = entry->header;
                    header.type = Header::Type::TIMEOUT;
                    entry->callbackResponse(header, entry->payload);
					entry = this->entries.remove(entry);
					continue;
				}
				else
				{
					backend->sendPacket(entry->header, entry->payload);

					entry->tries++;
					entry->time.restart(acknowledgeTimeout);
				}
			}

			++entry;
		}
		else
		{
			// WAIT_FOR_RESPONSE
			// Responses stay in the queue for ever if no response ever
			// comes. This may have to be changed.
			++entry;
		}
	}
}

// ----------------------------------------------------------------------------
void
xpcc::Dispatcher::addMessage(const Header& header,
		modm::SmartPointer& smartPayload)
{
	this->entries.append(Entry(header, smartPayload));
}

void
xpcc::Dispatcher::addMessage(const Header& header,
		modm::SmartPointer& smartPayload, ResponseCallback& responseCallback)
{
	this->entries.append(Entry(header, smartPayload, responseCallback));
}

void
xpcc::Dispatcher::addResponse(const Header& header,
		modm::SmartPointer& smartPayload)
{
	// it makes response more important, than requests
	// it prevents intern loops. Since it is possible to give a response while
	// an action is handled and call an action while response is handled
	// one component calling one action on another component on same board
	// handling its response in the same callback-function would cause a loop
	// if responses and actions both are appended at the tail of the list.
	// but now responses are handled in reverse order that's not good
	// what to do? a separator between responses and requests possible?

	this->entries.prepend(Entry(header, smartPayload));
}
