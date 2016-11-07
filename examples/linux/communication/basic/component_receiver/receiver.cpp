/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/debug/logger.hpp>

// set new log level
#undef XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::DEBUG

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
	XPCC_LOG_INFO << XPCC_FILE_INFO
			<< "action set position: x=" << parameter->x
			<< ", y=" << parameter->y << xpcc::endl;
	
	position = *parameter;
}

// ----------------------------------------------------------------------------
void
component::Receiver::actionGetPosition(const xpcc::ResponseHandle& handle)
{
	XPCC_LOG_INFO << XPCC_FILE_INFO << "action get position" << xpcc::endl;
	
	this->sendResponse(handle, position);
}

// ----------------------------------------------------------------------------
void
component::Receiver::update()
{
}
