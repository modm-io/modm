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

#include <modm/board/board.hpp>

#include <modm/processing/timer/periodic_timer.hpp>

#include <modm/communication/communication.hpp>
#include <modm/communication/xpcc/backend/can.hpp>

#include "component_odometry/odometry.hpp"

#include "communication/postman.hpp"
#include "communication/identifier.hpp"

using namespace Board;

namespace Led
{
	using B = LedBlue;
	using G = LedGreen;
	using R = LedRed;
}

/*
 * Periodically publish an MODM message on the CAN bus.
 * If it is longer than eight bytes it will fragmented and creates more than one CAN frame.
 *
 * Used in the ZeroMQ Example to generate MODM Odometry messages on the CAN bus.
 *
 * When rotating the encoder the x coordinate of the odometry message changes.
 *
 * Connect an Encoder:
 *   * GND to GND
 *   * A   to E9
 *   * B   to E11
 *
 * Connect a CAN transceiver
 *   * CTX to B9
 *   * CRX to B8
 *
 * CAN baud rate is 125 kBits
 */

static Can1 device;

// CanConnector does the fragmentation and defragmentation of xpcc messages to and from CAN frames.
static xpcc::CanConnector< Can1 > connector(&device);

// create an instance of the generated postman
Postman postman;

xpcc::Dispatcher dispatcher(&connector, &postman);

namespace component
{
	Odometry odometry(robot::component::ODOMETRY, dispatcher);
}

modm::ShortPeriodicTimer tmr(50);

int
main()
{
	Board::initialize();

	// Led::R::setOutput();
	// Led::G::setOutput();
	// Led::B::setOutput();

	using Timer = Timer1;
	using ChannelA = GpioInputE9;
	using ChannelB = GpioInputE11;
	auto ChannelAInputType = Gpio::InputType::PullUp;
	auto ChannelBInputType = Gpio::InputType::PullUp;

	// Initialze the encoder
	Timer::enable();
	Timer::setMode(Timer::Mode::UpCounter, Timer::SlaveMode::Encoder3);
	// Overflow must be 16bit because else a lot of our motor control code will break!
	Timer::setOverflow(0xffff);
	ChannelA::setInput(ChannelAInputType);
	ChannelB::setInput(ChannelBInputType);
	Timer::connect<ChannelA::Ch1, ChannelB::Ch2>();
	Timer::start();

	// Initialize Can1
	Can1::connect<GpioB8::Rx, GpioB9::Tx>(Gpio::InputType::PullUp);
	Can1::initialize<Board::systemClock, Can1::Bitrate::kBps125>(10);
	CanFilter::setFilter(0, CanFilter::FIFO0,
						CanFilter::ExtendedIdentifier(0),
						CanFilter::ExtendedFilterMask(0));


	LedGreen::set();

	while (true)
	{
		dispatcher.update();
		component::odometry.update();

		if (tmr.execute()) {
			LedOrange::toggle();
		}

		modm::delayMicroseconds(100);
	}

	return 0;
}
