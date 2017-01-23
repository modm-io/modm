// coding: utf-8
/* Copyright (c) 2017, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "connector.hpp"

namespace xpcc
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

	// Manual serialisation of XPCC Header and Payload into a byte buffer
	// The mapping of type, ack, dest, src and id into a uint32_t from
	// CanConnectorBase is used.

	constexpr uint16_t headerSize = 5;

	// Maximum valid size of xpcc::SmartPointer
	constexpr uint16_t maxPayloadSize = 65529;

	if(payload.getSize() > maxPayloadSize) {
		XPCC_LOG_ERROR << XPCC_FILE_INFO;
		XPCC_LOG_ERROR << "Trying to send message with invalid size: ";
		XPCC_LOG_ERROR << payload.getSize() << xpcc::endl;

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
const xpcc::SmartPointer
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

} // xpcc namespace
