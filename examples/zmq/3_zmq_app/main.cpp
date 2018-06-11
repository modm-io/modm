/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/debug/logger.hpp>
#include <modm/platform.hpp>

#include <modm/communication/xpcc/backend/zeromq/connector.hpp>

#include "component_gui/gui.hpp"

#include "communication/postman.hpp"
#include "communication/identifier.hpp"

/**
 * Simple subscriber with zeromq.
 *
 * The MODM Header and Payload is reconstructed from the zeromq message.
 */

// set new log level
#undef MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

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

		modm::delayMilliseconds(25);
	}
}
