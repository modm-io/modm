/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------- */

#ifndef COMPONENT__GUI_HPP
#define COMPONENT__GUI_HPP

#include <xpcc/communication/xpcc/abstract_component.hpp>
#include <xpcc/processing/timer.hpp>

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
