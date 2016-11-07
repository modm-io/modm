/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/debug/logger.hpp>
#include <xpcc/architecture.hpp>

#include <xpcc/communication.hpp>
#include <xpcc/communication/xpcc/backend/zeromq/connector.hpp>

#include "component_gui/gui.hpp"

#include "communication/postman.hpp"
#include "communication/identifier.hpp"

/**
 * Simple subscriber with zeromq.
 *
 * The XPCC Header and Payload is reconstructed from the zeromq message.
 */

// set new log level
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

const std::string endpointIn  = "tcp://127.0.0.1:8211";
const std::string endpointOut = "tcp://127.0.0.1:8212";
static xpcc::ZeroMQConnector connector(endpointIn, endpointOut, xpcc::ZeroMQConnector::Mode::SubPush);

// create an instance of the generated postman
Postman postman;

xpcc::Dispatcher dispatcher(&connector, &postman);

namespace component
{
	Gui gui(robot::component::GUI, dispatcher);
}

int
main()
{
	while(true)
	{
		dispatcher.update();
		component::gui.update();

		xpcc::delayMilliseconds(25);
	}
}
