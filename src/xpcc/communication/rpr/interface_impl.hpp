/*

 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_RPR_INTERFACE_HPP
#	error	"Don't include this file directly, use 'interface.hpp' instead!"
#endif


#ifndef MODM_RPR_DEBUG
#	define MODM_RPR_DEBUG 0
#endif

#include <cstring>

#if MODM_RPR_DEBUG
#	include <modm/debug/logger.hpp>
#	undef MODM_LOG_LEVEL
#	define MODM_LOG_LEVEL	modm::log::DEBUG
#	define MODM_RPR_LOG(x) MODM_LOG_DEBUG << x << modm::endl
#else
#	define MODM_RPR_LOG(x)
#endif

// ----------------------------------------------------------------------------
template <typename Device, std::size_t N>
uint16_t modm::rpr::Interface<Device, N>::_address = 0;

template <typename Device, std::size_t N>
uint16_t modm::rpr::Interface<Device, N>::_groupAddress = 0x7fff;


template <typename Device, std::size_t N>
typename modm::rpr::Interface<Device, N>::Queue
modm::rpr::Interface<Device, N>::messagesToSend;

template <typename Device, std::size_t N>
typename modm::rpr::Interface<Device, N>::Queue
modm::rpr::Interface<Device, N>::receivedMessages;


template <typename Device, std::size_t N>
modm::rpr::Message
modm::rpr::Interface<Device, N>::receiveBuffer;

template <typename Device, std::size_t N>
uint8_t modm::rpr::Interface<Device, N>::rx_buffer[N+8];


template <typename Device, std::size_t N>
uint16_t modm::rpr::Interface<Device, N>::crc = crcInitialValue;

template <typename Device, std::size_t N>
uint8_t modm::rpr::Interface<Device, N>::length = 0;

template <typename Device, std::size_t N>
bool modm::rpr::Interface<Device, N>::nextEscaped = false;


template <typename Device, std::size_t N>
uint8_t modm::rpr::Interface<Device, N>::addressBuffer = 0;

template <typename Device, std::size_t N>
uint8_t modm::rpr::Interface<Device, N>::status = 0;

// ----------------------------------------------------------------------------
template <typename Device, std::size_t N>
void
modm::rpr::Interface<Device, N>::initialize(uint16_t address, uint16_t groupAddress)
{
	_address = address & ADDRESS_VALUE;
	_groupAddress = groupAddress & ADDRESS_VALUE;
	receiveBuffer.payload = rx_buffer;
	status = 0;
}

// ----------------------------------------------------------------------------
template <typename Device, std::size_t N>
bool
modm::rpr::Interface<Device, N>::sendMessage(uint16_t destination,
											   MessageType type,
											   uint8_t command,
											   const void *payload,
											   std::size_t payloadLength)
{
	Message message;
	
	message.type = type;
	message.destination = destination;
	message.source = _address;
	message.command = command;
	message.payload = static_cast<uint8_t *>(const_cast<void *>(payload));
	message.length = payloadLength;
	
	return sendMessage(&message);
}

template <typename Device, std::size_t N>
bool
modm::rpr::Interface<Device, N>::sendMessage(Message *message)
{
	if (status & STATUS_START_DELIMITER_RECEIVED)
	{
		// bus is busy, copy the data
		// this copies the data internally
		if (pushMessage(messagesToSend, message))
		{
			// get a pointer to the copy of the message
			return true;
		}
	}
	else
	{
		// bus is not busy, send message
		writeMessage(message);
	}
	return false;
}

// ----------------------------------------------------------------------------

template <typename Device, std::size_t N>
modm::rpr::Message *
modm::rpr::Interface<Device, N>::getReceivedMessage()
{
	return getMessage(receivedMessages);
}

template <typename Device, std::size_t N>
void
modm::rpr::Interface<Device, N>::dropReceivedMessage()
{
	popMessage(receivedMessages);
}

// ----------------------------------------------------------------------------
template <typename Device, std::size_t N>
void
modm::rpr::Interface<Device, N>::update()
{
	uint8_t data;
	
	if (status & STATUS_END_DELIMITER_RECEIVED && !messagesToSend.isEmpty())
	{
		writeMessage(getMessage(messagesToSend));
		popMessage(messagesToSend);
	}
	
	while (Device::read(data))
	{
		MODM_RPR_LOG("receiving raw " << modm::hex << data << modm::ascii);
		
		if (data == startDelimiterByte)
		{
			status &= ~STATUS_END_DELIMITER_RECEIVED;
			status |= STATUS_START_DELIMITER_RECEIVED;

			MODM_RPR_LOG("start delimiter");
			
			crc = crcInitialValue;
			length = 0;
			nextEscaped = false;
			
			// we do not send the frame boundaries here, but wait for the AC.
		}
		else if (data == endDelimiterByte)
		{
			if (length >= 6 && (status & STATUS_START_DELIMITER_RECEIVED))
			{
				status &= ~STATUS_START_DELIMITER_RECEIVED;
				status |= STATUS_END_DELIMITER_RECEIVED;
				
				if (!(status & STATUS_SOURCE_RECOGNISED) && (receiveBuffer.type != MESSAGE_TYPE_UNICAST))
				{
					MODM_RPR_LOG("tx: forwarding endDelimiterByte");
					Device::write(endDelimiterByte);
				}
				MODM_RPR_LOG("end delimiter with length=" << length);
				
				if (status & STATUS_DESTINATION_RECOGNISED)
				{
					if (crc == 0)
					{
						MODM_RPR_LOG("crc check success");
						if (receiveBuffer.length > 1)
						{
							receiveBuffer.command = receiveBuffer.payload[0];
							receiveBuffer.payload += 1;
							receiveBuffer.length -= 1;
						}
						pushMessage(receivedMessages, &receiveBuffer);
						receiveBuffer.payload = rx_buffer;
					}
					else {
						MODM_RPR_LOG("crc check failure");
					}
				}
			}
			crc = crcInitialValue;
			length = 0;
			nextEscaped = false;
		}
		else if (data == controlEscapeByte)
		{
			// the next byte is escaped
			nextEscaped = true;
			MODM_RPR_LOG("escape sequence");
			continue;
		}
		else
		{
			if (nextEscaped)
			{
				nextEscaped = false;
				// toggle bit 5
				data = data ^ 0x20;
				MODM_RPR_LOG("data escaped");
			}
			// all data is now escaped
			
			// make sure we actually received a start delimiter before the payload
			if (!(status & STATUS_START_DELIMITER_RECEIVED))
				return;
			
			switch (length++)
			{
				// LSB of destination address
				case 0:
					MODM_RPR_LOG("rx: LSB dest");
					addressBuffer = data;
					break;
					
					// MSB of destination address
				case 1:
				{
					MODM_RPR_LOG("rx: MSB dest");
					// check the destination address against our own
					uint16_t dest = (data << 8) | addressBuffer;
					status &= ~(STATUS_DESTINATION_RECOGNISED | STATUS_RX_BUFFER_OVERFLOW | STATUS_SOURCE_RECOGNISED);
					receiveBuffer.type = MESSAGE_TYPE_ANY;
					receiveBuffer.destination = dest;
					receiveBuffer.length = 0;
					
					// it is a broadcast, we need to listen
					if (receiveBuffer.destination == ADDRESS_BROADCAST)
					{
						MODM_RPR_LOG("rx: broadcast");
						receiveBuffer.type = MESSAGE_TYPE_BROADCAST;
						status |= STATUS_DESTINATION_RECOGNISED;
					}
					else
					{
						if (receiveBuffer.destination & ADDRESS_INDIVIDUAL_GROUP)
						{
							// group address
							if (_groupAddress == (receiveBuffer.destination & ADDRESS_VALUE))
							{
								MODM_RPR_LOG("rx: my group");
								receiveBuffer.type = MESSAGE_TYPE_MULTICAST;
								status |= STATUS_DESTINATION_RECOGNISED;
							}
						}
						else {
							// individual address
							if (_address == (receiveBuffer.destination & ADDRESS_VALUE))
							{
								MODM_RPR_LOG("rx: my address");
								receiveBuffer.type = MESSAGE_TYPE_UNICAST;
								status |= STATUS_DESTINATION_RECOGNISED;
							}
						}
					}
					
					if (status & STATUS_DESTINATION_RECOGNISED)
					{
						crc = crcUpdate(crc, addressBuffer);
						crc = crcUpdate(crc, data);
					}
				}
					break;
					
					// LSB of source address
				case 2:
					MODM_RPR_LOG("rx: LSB source");
					addressBuffer = data;
					break;
					
					// MSB of Source Address
				case 3:
				{
					MODM_RPR_LOG("rx: MSB source");
					// check the source address against our own
					uint16_t source = (data << 8) | addressBuffer;
					receiveBuffer.source = (source & ADDRESS_VALUE);
					
					if (_address == receiveBuffer.source)
					{
						status |= STATUS_SOURCE_RECOGNISED;
					}
					
					if (status & STATUS_DESTINATION_RECOGNISED)
					{
						crc = crcUpdate(crc, addressBuffer);
						crc = crcUpdate(crc, data);
					}
					
					if (!(status & STATUS_SOURCE_RECOGNISED) && (receiveBuffer.type != MESSAGE_TYPE_UNICAST))
					{
						MODM_RPR_LOG("tx: forwarding destination");
						Device::write(startDelimiterByte);
						writeByteEscaped(receiveBuffer.destination);
						writeByteEscaped(receiveBuffer.destination >> 8);
						
						MODM_RPR_LOG("tx: forwarding source");
						writeByteEscaped(addressBuffer);
						writeByteEscaped(data);
					}
					else {
						MODM_RPR_LOG("rx: no forwarding");
					}

				}
					break;
					
				default:
					if (status & STATUS_DESTINATION_RECOGNISED)
					{
						if (length <= N+8)
						{
							MODM_RPR_LOG("rx: buffering payload");
							receiveBuffer.payload[length-5] = data;
							receiveBuffer.length++;
							crc = crcUpdate(crc, data);
						}
						else {
							// really, really bad programmer !
							// now go sit in the corner and increase dat payload buffer
							status |= STATUS_RX_BUFFER_OVERFLOW;
							MODM_RPR_LOG("rx: buffer overflow!!!");
						}
					}
					
					if (!(status & STATUS_SOURCE_RECOGNISED) && (receiveBuffer.type != MESSAGE_TYPE_UNICAST))
					{
						MODM_RPR_LOG("forwarding payload");
						writeByteEscaped(data);
					}
					break;
			}
		}
	}
}

// ----------------------------------------------------------------------------
template <typename Device, std::size_t N>
void
modm::rpr::Interface<Device, N>::writeByteEscaped(uint8_t data)
{
	if (data == startDelimiterByte || data == endDelimiterByte || data == controlEscapeByte)
	{
		MODM_RPR_LOG("tx: " << modm::hex << controlEscapeByte << modm::ascii);
		MODM_RPR_LOG("tx: " << modm::hex << (data ^ 0x20) << modm::ascii);
		Device::write(controlEscapeByte);
		Device::write(data ^ 0x20);		// toggle bit 5
	}
	else
	{
		MODM_RPR_LOG("tx: " << modm::hex << data << modm::ascii);
		Device::write(data);
	}
}

template <typename Device, std::size_t N>
void
modm::rpr::Interface<Device, N>::writeMessage(Message *message)
{
	uint16_t crc = crcInitialValue;
	
	// Start Delimiter
	Device::write(startDelimiterByte);
	
	// HEADER
	uint16_t destination = message->destination;
	// Destination Address
	if (message->type == MESSAGE_TYPE_BROADCAST)
	{
		destination = ADDRESS_BROADCAST;
	}
	else {
		destination &= ADDRESS_VALUE;
		if (message->type == MESSAGE_TYPE_MULTICAST)
			destination |= ADDRESS_INDIVIDUAL_GROUP;
	}
	
	writeByteEscaped(destination);
	crc = crcUpdate(crc, destination);
	writeByteEscaped(destination >> 8);
	crc = crcUpdate(crc, destination >> 8);
	
	// Source Address
	// LSB first
	writeByteEscaped(message->source);
	crc = crcUpdate(crc, message->source);
	writeByteEscaped(message->source >> 8);
	crc = crcUpdate(crc, message->source >> 8);
	
	// PAYLOAD
	// Command
	writeByteEscaped(message->command);
	crc = crcUpdate(crc, message->command);
	
	// Optional Payload
	std::size_t i = 0;
	while (i < message->length)
	{
		writeByteEscaped(message->payload[i]);
		crc = crcUpdate(crc, message->payload[i]);
		i++;
	}
	
	// FCS
	writeByteEscaped(crc);
	writeByteEscaped(crc >> 8);
	
	// End Delimiter
	Device::write(endDelimiterByte);
	
	popMessage(messagesToSend);
}

// ----------------------------------------------------------------------------
template <typename Device, std::size_t N>
bool
modm::rpr::Interface<Device, N>::pushMessage(Queue &queue, Message *message)
{
	if (queue.isFull()) {
		MODM_RPR_LOG("queue full");
		return false;
	}
	MODM_RPR_LOG("allocating");
	// manually create a new buffer
	uint8_t *newBuffer = bufferAllocator.allocate(message->length);
	if (newBuffer)
	{
		MODM_RPR_LOG("allocated " << message->length << " uint8_t");
		// copy the content into the new buffer
		std::memcpy(newBuffer, message->payload, message->length);
		MODM_RPR_LOG("copying payload");
		// point the message payload pointer to the new buffer
		message->payload = newBuffer;
		// push it into the list, which will copy the rest
		queue.push(*message);
		MODM_RPR_LOG("pushing");
		return true;
	}
	return false;
}

template <typename Device, std::size_t N>
void
modm::rpr::Interface<Device, N>::popMessage(Queue &queue)
{
	if (!queue.isEmpty())
	{
		// deallocate the external buffer
		Message message = queue.get();
		MODM_RPR_LOG("deallocating");
		bufferAllocator.deallocate(message.payload);
		// then remove the rest
		queue.pop();
	}
}

template <typename Device, std::size_t N>
modm::rpr::Message *
modm::rpr::Interface<Device, N>::getMessage(Queue &queue)
{
	if (queue.isEmpty())
		return 0;
	
	return &queue.get();
}

template <typename Device, std::size_t N>
bool
modm::rpr::Interface<Device, N>::moveMessage(Queue &destination, Queue &source)
{
	// do not reallocate the payload buffer!
	// just move what is essential
	if (source.isEmpty())
		return false;
	
	MODM_RPR_LOG("moving");
	destination.push(source.get());
	source.pop();
	return true;
}
