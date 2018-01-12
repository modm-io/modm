/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, 2011, Georgi Grinshpun
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2013, 2016, Sascha Schade
 * Copyright (c) 2012, 2015, 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef COMPONENT_SENDER_HPP
#define COMPONENT_SENDER_HPP

#include <modm/communication/xpcc/abstract_component.hpp>
#include <modm/processing/timer.hpp>

#include "communication/packets.hpp"

namespace component
{
	class Sender : public xpcc::AbstractComponent
	{
	public:
		Sender(uint8_t id, xpcc::Dispatcher &communication);
		
		void
		update();
		
	private:
		void
		getPositionCallback(const xpcc::Header& header,
				const robot::packet::Position *parameter);
		
		xpcc::ResponseCallback positionCallback;
		modm::ShortPeriodicTimer timer;
	};
}

#endif // COMPONENT_SENDER_HPP
