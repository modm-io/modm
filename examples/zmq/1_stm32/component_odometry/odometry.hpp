/*
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef COMPONENT_ODOMETRY_HPP
#define COMPONENT_ODOMETRY_HPP

#include <modm/communication/xpcc/abstract_component.hpp>
#include <modm/processing/timer.hpp>

#include "communication/packets.hpp"

namespace component
{
	class Odometry : public xpcc::AbstractComponent
	{
	public:
		Odometry(uint8_t id, xpcc::Dispatcher &communication);
		
		void
		update();

	public:
		// Callback for modm
		void actionSetLedRed(const xpcc::ResponseHandle& responseHandle, const robot::packet::Bool *payload);
		
	private:
		modm::ShortPeriodicTimer timer;
	};
}

#endif // COMPONENT_ODOMETRY_HPP
