/* Copyright (c) 2016, Roboterclub Aachen e. V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 * ------------------------------------------------------------------------- */

#ifndef COMPONENT__ODOMETRY_HPP
#define COMPONENT__ODOMETRY_HPP

#include <xpcc/communication/xpcc/abstract_component.hpp>
#include <xpcc/processing/timer.hpp>

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
		// Callback for xpcc
		void actionSetLedRed(const xpcc::ResponseHandle& responseHandle, const robot::packet::Bool *payload);
		
	private:
		xpcc::ShortPeriodicTimer timer;
	};
}

#endif // COMPONENT__ODOMETRY_HPP
