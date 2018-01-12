/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Christopher Durand
 * Copyright (c) 2017, Niklas Hauser
 * Copyright (c) 2018, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_ZEROMQ_CONNECTOR_HPP
#define	XPCC_ZEROMQ_CONNECTOR_HPP

#include <cstring>		// for std::memcpy

#include <zmqpp/zmqpp.hpp>

#include <modm/debug/logger.hpp>
#undef MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::ERROR

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

	~ZeroMQConnector() override;

	virtual void
	sendPacket(const Header &header, modm::SmartPointer payload) override;

	virtual bool
	isPacketAvailable() const override;

	virtual const Header&
	getPacketHeader() const override;

	virtual const modm::SmartPointer
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

#endif // XPCC_ZEROMQ_CONNECTOR_HPP
