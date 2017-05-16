/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, 2011, Georgi Grinshpun
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2013, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef COMPONENT_RECEIVER_HPP
#define COMPONENT_RECEIVER_HPP

#include <modm/communication/modm/abstract_component.hpp>

#include "communication/packets.hpp"

namespace component
{
	class Receiver : public modm::AbstractComponent
	{
	public:
		Receiver(uint8_t id, modm::Dispatcher &communication);
		
		void
		actionSetPosition(const modm::ResponseHandle& handle,
				const robot::packet::Position *parameter);
		
		void
		actionGetPosition(const modm::ResponseHandle& handle);
		
		void
		update();
		
	private:
		robot::packet::Position position;
	};
}

#endif // COMPONENT_RECEIVER_HPP
