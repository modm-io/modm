/*
 * Copyright (c) 2009-2011, 2014, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2010-2011, Georgi Grinshpun
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2012, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_CAN_CONNECTOR_HPP
	#error	"Don't include this file directly, use 'can_connector.hpp' instead!"
#endif

#include <modm/math/utils/bit_operation.hpp>
#include <modm/architecture/interface/can_message.hpp>

// ----------------------------------------------------------------------------
template<typename Driver>
xpcc::CanConnector<Driver>::CanConnector(Driver *driver) :
	canDriver(driver)
{
}

template<typename Driver>
xpcc::CanConnector<Driver>::~CanConnector()
{
}

// ----------------------------------------------------------------------------
template<typename Driver>
bool
xpcc::CanConnector<Driver>::isPacketAvailable() const
{
	return !this->receivedMessages.isEmpty();
}

template<typename Driver>
const xpcc::Header&
xpcc::CanConnector<Driver>::getPacketHeader() const
{
	return this->receivedMessages.getFront().header;
}

template<typename Driver>
const modm::SmartPointer
xpcc::CanConnector<Driver>::getPacketPayload() const
{
	return this->receivedMessages.getFront().payload;
}

// ----------------------------------------------------------------------------
template<typename Driver>
void
xpcc::CanConnector<Driver>::sendPacket(const Header &header, modm::SmartPointer payload)
{
	bool successful = false;
	bool fragmented = (payload.getSize() > 8);

	uint32_t identifier = convertToIdentifier(header, fragmented);
	if (!fragmented && this->canDriver->isReadyToSend())
	{
		// try to send the message directly
		successful = this->sendMessage(identifier,
				payload.getPointer(), payload.getSize());
	}

	if (!successful)
	{
		// append the message to the list of waiting messages
		this->sendList.append(SendListItem(identifier, payload));
	}
}

// ----------------------------------------------------------------------------
template<typename Driver>
void
xpcc::CanConnector<Driver>::dropPacket()
{
	this->receivedMessages.removeFront();
}

// ----------------------------------------------------------------------------
template<typename Driver>
void
xpcc::CanConnector<Driver>::update()
{
	while (this->canDriver->isMessageAvailable()) {
		this->retrieveMessage();
	}
	this->sendWaitingMessages();
}

// ----------------------------------------------------------------------------
// protected
// ----------------------------------------------------------------------------

template<typename Driver>
bool
xpcc::CanConnector<Driver>::sendMessage(const uint32_t & identifier,
		const uint8_t *data, uint8_t size)
{
	modm::can::Message message(identifier, size);

	// copy payload data
	std::memcpy(message.data, data, size);

	return this->canDriver->sendMessage(message);
}

template<typename Driver>
void
xpcc::CanConnector<Driver>::sendWaitingMessages()
{
	if (this->sendList.isEmpty()) {
		// no message in the queue
		return;
	}
	else if (canDriver->getBusState() != Driver::BusState::Connected) {
		// No connection to the CAN bus, drop all messages which should be send
		while (!sendList.isEmpty()) {
			sendList.removeFront();
		}
		return;
	}

	SendListItem& message = this->sendList.getFront();

	uint8_t messageSize = message.payload.getSize();
	if (messageSize > 8)
	{
		// fragmented message
		uint8_t data[8];

		data[0] = message.fragmentIndex | (this->messageCounter & 0xf0);
		data[1] = messageSize; 	// size of the complete message

		bool sendFinished = true;
		uint8_t offset = message.fragmentIndex * 6;
		uint8_t fragmentSize = messageSize - offset;
		if (fragmentSize > 6)
		{
			fragmentSize = 6;
			sendFinished = false;
		}
		// otherwise fragmentSize is smaller or equal to six, so the last
		// fragment is about to be sent.

		memcpy(data + 2, message.payload.getPointer() + offset, fragmentSize);

		if (sendMessage(message.identifier, data, fragmentSize + 2))
		{
			message.fragmentIndex++;
			if (sendFinished)
			{
				// message was the last fragment
				// => remove it from the list
				this->sendList.removeFront();
				this->messageCounter += 0x10;
			}
		}
	}
	else
	{
		if (this->sendMessage(message.identifier, message.payload.getPointer(),
				messageSize))
		{
			this->sendList.removeFront();
		}
	}
}

template<typename Driver>
bool
xpcc::CanConnector<Driver>::retrieveMessage()
{
	modm::can::Message message;
	if (this->canDriver->getMessage(message))
	{
		xpcc::Header header;
		bool isFragment = convertToHeader(message.identifier, header);

		if (!isFragment)
		{
			this->receivedMessages.append(ReceiveListItem(message.length, header));
			std::memcpy(this->receivedMessages.getBack().payload.getPointer(),
					message.data,
					message.length);
		}
		else
		{
			// find existing container otherwise create a new one
			const uint8_t fragmentIndex = message.data[0] & 0x0f;
			const uint8_t counter = message.data[0] & 0xf0;
			const uint8_t messageSize = message.data[1];

			// calculate the number of messages need to send messageSize-bytes
			uint8_t numberOfFragments = this->getNumberOfFragments(messageSize);

			if (message.length < 3 || messageSize > 48 ||
					fragmentIndex >= numberOfFragments)
			{
				// illegal format:
				//   fragmented messages need to have at least 3 byte payload,
				// 	 the maximum size is 48 Bytes and the fragment number
				//	 should not be higher than the number of fragments.
				return false;
			}

			// check the length of the fragment (all fragments except the
			// last one need to have a payload-length of 6 bytes + 2 byte
			// fragment information)
			uint8_t offset = fragmentIndex * 6;
			if (fragmentIndex + 1 == numberOfFragments)
			{
				// this one is the last fragment
				if (messageSize - offset != message.length - 2)
				{
					// illegal format
					return false;
				}
			}
			else if (message.length != 8)
			{
				// illegal format
				return false;
			}

			// Check if other parts of this message are already in the
			// list of receiving messages.
			typename ReceiveList::iterator packet = this->pendingMessages.begin();
			for ( ; packet != this->pendingMessages.end(); ++packet)
			{
				if (packet->header == header &&
					packet->counter == counter)
				{
					break;
				}
			}

			if (packet == this->pendingMessages.end()) {
				// message not found => first part of this message,
				// prepend it to the list
				this->pendingMessages.prepend(ReceiveListItem(messageSize, header, counter));
				packet = this->pendingMessages.begin();
			}

			// create a marker for the currently received fragment and
			// test if the fragment was already received
			const uint8_t currentFragment = (1 << fragmentIndex);
			if (currentFragment & packet->receivedFragments)
			{
				// error: received fragment twice -> most likely a new message -> delete the old one
				//MODM_LOG_WARNING << "lost fragment" << modm::flush;
				packet->receivedFragments = 0;
			}
			packet->receivedFragments |= currentFragment;

			std::memcpy(packet->payload.getPointer() + offset,
					message.data + 2,
					message.length - 2);

			// test if this was the last segment, otherwise we have to wait
			// for more messages
			if (modm::bitCount(packet->receivedFragments) == numberOfFragments)
			{
				this->receivedMessages.append(*packet);
				this->pendingMessages.remove(packet);
			}
		}

		return true;
	}
	else {
		return false;
	}
}
