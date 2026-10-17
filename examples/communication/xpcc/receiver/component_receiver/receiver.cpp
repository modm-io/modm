/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/debug/logger.hpp>

// set new log level
#undef MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

#include "receiver.hpp"

// ----------------------------------------------------------------------------
component::Receiver::Receiver(uint8_t id, xpcc::Dispatcher &communication) :
	xpcc::AbstractComponent(id, communication)
{
}

// ----------------------------------------------------------------------------
void
component::Receiver::actionSetPosition(const xpcc::ResponseHandle&,
		const robot::packet::Position *parameter)
{
	MODM_LOG_INFO << MODM_FILE_INFO
			<< "action set position: x=" << parameter->x
			<< ", y=" << parameter->y << modm::endl;

	position = *parameter;
}

// ----------------------------------------------------------------------------
void
component::Receiver::actionGetPosition(const xpcc::ResponseHandle& handle)
{
	MODM_LOG_INFO << MODM_FILE_INFO << "action get position" << modm::endl;

	this->sendResponse(handle, position);
}

// ----------------------------------------------------------------------------
void
component::Receiver::update()
{
}
