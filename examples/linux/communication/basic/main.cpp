/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2013-2014, Sascha Schade
 * Copyright (c) 2014, 2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/architecture.hpp>

#include <modm/communication/communication.hpp>
// #include <modm/communication/modm/backend/tipc.hpp>
#include <modm/communication/modm/backend/zeromq.hpp>

#include <modm/debug/logger.hpp>

// set new log level
#undef MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

#include "component_receiver/receiver.hpp"
#include "component_sender/sender.hpp"

#include "communication/postman.hpp"
#include "communication/identifier.hpp"

// Use TIPC on Linux only
// modm::TipcConnector connector;

// Use ZeroMQ on Linux and Darwin
const std::string endpointIn  = "tcp://127.0.0.1:8211";
const std::string endpointOut = "tcp://127.0.0.1:8212";
static modm::ZeroMQConnector connector(endpointIn, endpointOut, modm::ZeroMQConnector::Mode::SubPush);

// create an instance of the generated postman
Postman postman;

modm::Dispatcher dispatcher(&connector, &postman);

namespace component
{
	Sender sender(robot::component::SENDER, dispatcher);
	Receiver receiver(robot::component::RECEIVER, dispatcher);
}

int
main()
{
	// Required for TIPC on Linux only
	// connector.addReceiverId(robot::component::SENDER);
	// connector.addReceiverId(robot::component::RECEIVER);
	
	MODM_LOG_INFO << "Welcome to the communication test!" << modm::endl; 
	
	while (true)
	{
		// deliver received messages
		dispatcher.update();
		
		component::receiver.update();
		component::sender.update();
		
		modm::delayMicroseconds(100);
	}
}
