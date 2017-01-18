// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__ZEROMQ_CONNECTOR_HPP
#define	XPCC__ZEROMQ_CONNECTOR_HPP

#include <cstring>		// for std::memcpy

#include <zmqpp/zmqpp.hpp>

#include <xpcc/debug/logger.hpp>
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::ERROR

#include "../backend_interface.hpp"
#include "reader.hpp"

namespace xpcc
{
class ZeroMQConnectorBase
{
public:
	enum class Mode
	{
		SubPush, /// In this mode the backend connects to a remote machine.
		PubPull, /// Server mode in which the backend binds to two ports. The ports must be accessible.
	};
};

class ZeroMQConnector : public ZeroMQConnectorBase, public BackendInterface
{
public:
	ZeroMQConnector(std::string endpointIn, std::string endpointOut, Mode mode = Mode::SubPush) :
		socketIn (context, (mode == Mode::SubPush ? zmqpp::socket_type::sub  : zmqpp::socket_type::pull)),
		socketOut(context, (mode == Mode::SubPush ? zmqpp::socket_type::push : zmqpp::socket_type::pub)),
		reader(socketIn)
	{
		switch(mode)
		{
			case
			Mode::SubPush:
				this->socketIn.connect(endpointIn);
				this->socketIn.subscribe("");

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

	virtual
	~ZeroMQConnector()
	{
		this->reader.stop();
		this->socketIn.unsubscribe("");
		this->socketOut.unsubscribe("");
		this->socketIn.close();
		this->socketOut.close();
	}

	virtual void
	sendPacket(const Header &header, SmartPointer payload)
	{
		// Publish the reassembled message with zeromq
		zmqpp::message message;

		// Manual serialisation of XPCC Header and Payload into a byte buffer
		// The mapping of type, ack, dest, src and id into a uint32_t from
		// CanConnectorBase is used.

		// 5 bytes for header
		uint16_t buf_size = 5 + payload.getSize();
		uint8_t buf[buf_size];

		buf[0] = static_cast<uint8_t>(header.type);
		buf[1] = header.isAcknowledge;
		buf[2] = header.destination;
		buf[3] = header.source;
		buf[4] = header.packetIdentifier;

		memcpy(buf + 5, payload.getPointer(), payload.getSize());

#		if ZMQPP_VERSION_MAJOR >= 4
		/**
		 * Breaking change in 4.1.1:
		 * Removed message::add(pointer, size_t) as there were situations it conflicts with the new easier 
		 * to use templated add. This has been replaced with a message::add_raw(pointer, size_t) method.
		 * https://github.com/zeromq/zmqpp/blob/develop/CHANGES.md
		 */
		message.add_raw(buf, buf_size);
#		else
		message.add(buf, buf_size);
#		endif

		socketOut.send(message, /* dont_block = */ true);
	}

	virtual bool
	isPacketAvailable() const
	{
		return this->reader.isPacketAvailable();
	}

	virtual const Header&
	getPacketHeader() const
	{
		return this->reader.getPacket().header;
	};

	virtual const xpcc::SmartPointer
	getPacketPayload() const
	{
		return this->reader.getPacket().payload;
	};

	virtual void
	dropPacket()
	{
		this->reader.dropPacket();
	}

	virtual void
	update()
	{
		// Nothing to do here
	}
protected:
	zmqpp::context context;
	zmqpp::socket socketIn;
	zmqpp::socket socketOut;

	ZeroMQReader reader;
};

} // xpcc namespace

#endif // XPCC__ZEROMQ_CONNECTOR_HPP
