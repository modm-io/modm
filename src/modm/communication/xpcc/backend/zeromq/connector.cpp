// coding: utf-8
/*
 * Copyright (c) 2017, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "connector.hpp"

namespace modm
{

ZeroMQConnector::ZeroMQConnector(std::string endpointIn, std::string endpointOut, Mode mode) :
	socketIn (context, (mode == Mode::SubPush ? zmqpp::socket_type::sub  : zmqpp::socket_type::pull)),
	socketOut(context, (mode == Mode::SubPush ? zmqpp::socket_type::push : zmqpp::socket_type::pub)),
	reader(socketIn)
{
	switch(mode)
	{
		case
		Mode::SubPush:
			this->socketIn.connect(endpointIn);

			if(this->socketIn.type() == zmqpp::socket_type::sub) {
				this->socketIn.subscribe("");
			}

			this->socketOut.connect(endpointOut);
		break;

		case
		Mode::PubPull:
			this->socketIn.bind(endpointIn);
			this->socketOut.bind(endpointOut);
		break;
	}

	this->reader.start();
}

// ----------------------------------------------------------------------------
ZeroMQConnector::~ZeroMQConnector()
{
	this->reader.stop();

	if(this->socketIn.type() == zmqpp::socket_type::sub) {
		this->socketIn.unsubscribe("");
	}

	this->socketIn.close();
	this->socketOut.close();
}

// ----------------------------------------------------------------------------
void
ZeroMQConnector::sendPacket(const Header &header, SmartPointer payload)
{
	// Publish the reassembled message with zeromq
	zmqpp::message message;

	// Manual serialisation of MODM Header and Payload into a byte buffer
	// The mapping of type, ack, dest, src and id into a uint32_t from
	// CanConnectorBase is used.

	constexpr uint16_t headerSize = 5;

	// Maximum valid size of modm::SmartPointer
	constexpr uint16_t maxPayloadSize = 65529;

	if(payload.getSize() > maxPayloadSize) {
		MODM_LOG_ERROR << MODM_FILE_INFO;
		MODM_LOG_ERROR << "Trying to send message with invalid size: ";
		MODM_LOG_ERROR << payload.getSize() << modm::endl;

		return;
	}

	const std::size_t buf_size = headerSize + payload.getSize();
	uint8_t buf[buf_size];

	buf[0] = static_cast<uint8_t>(header.type);
	buf[1] = header.isAcknowledge;
	buf[2] = header.destination;
	buf[3] = header.source;
	buf[4] = header.packetIdentifier;

	memcpy(buf + headerSize, payload.getPointer(), payload.getSize());

#	if ZMQPP_VERSION_MAJOR >= 4
	/**
	* Breaking change in 4.1.1:
	* Removed message::add(pointer, size_t) as there were situations it conflicts with the new easier 
	* to use templated add. This has been replaced with a message::add_raw(pointer, size_t) method.
	* https://github.com/zeromq/zmqpp/blob/develop/CHANGES.md
	*/
	message.add_raw(buf, buf_size);
#	else
	message.add(buf, buf_size);
#	endif

	socketOut.send(message, /* dont_block = */ true);
}

// ----------------------------------------------------------------------------
bool
ZeroMQConnector::isPacketAvailable() const
{
	return this->reader.isPacketAvailable();
}

// ----------------------------------------------------------------------------
const Header&
ZeroMQConnector::getPacketHeader() const
{
	return this->reader.getPacket().header;
};

// ----------------------------------------------------------------------------
const modm::SmartPointer
ZeroMQConnector::getPacketPayload() const
{
	return this->reader.getPacket().payload;
};

// ----------------------------------------------------------------------------
void
ZeroMQConnector::dropPacket()
{
	this->reader.dropPacket();
}

// ----------------------------------------------------------------------------
void
ZeroMQConnector::update()
{
}

} // modm namespace
