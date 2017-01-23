// coding: utf-8
/* Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Christopher Durand
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

/**
 * @brief	ZeroMQ communication backend for hosted
 *
 * @ingroup	backend
 *
 * @author	strongly-typed
 * @author	Christopher Durand <christopher.durand@rwth-aachen.de>
 */
class ZeroMQConnector : public ZeroMQConnectorBase, public BackendInterface
{
public:
	ZeroMQConnector(std::string endpointIn, std::string endpointOut,
					Mode mode = Mode::SubPush);

	virtual
	~ZeroMQConnector() override;

	virtual void
	sendPacket(const Header &header, SmartPointer payload) override;

	virtual bool
	isPacketAvailable() const override;

	virtual const Header&
	getPacketHeader() const override;

	virtual const xpcc::SmartPointer
	getPacketPayload() const override;

	virtual void
	dropPacket() override;

	virtual void
	update() override;

protected:
	zmqpp::context context;
	zmqpp::socket socketIn;
	zmqpp::socket socketOut;

	ZeroMQReader reader;
};

} // xpcc namespace

#endif // XPCC__ZEROMQ_CONNECTOR_HPP
