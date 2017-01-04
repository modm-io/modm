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

#ifndef COMPONENT__GUI_HPP
#define COMPONENT__GUI_HPP

#include <modm/communication/xpcc/abstract_component.hpp>
#include <modm/processing/timer.hpp>

#include "communication/packets.hpp"

namespace component
{
	class Gui : public xpcc::AbstractComponent
	{
	public:
		Gui(uint8_t id, xpcc::Dispatcher &communication);
		
		void
		update();

		void
		eventRobotLocation(xpcc::Header header, const robot::packet::Location * const payload);
	};
}

#endif // COMPONENT__GUI_HPP
