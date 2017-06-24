/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012-2014, 2016-2017, Niklas Hauser
 * Copyright (c) 2014, 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/platform.hpp>
#include <modm/communication/communication.hpp>
#include <modm/communication/xpcc/backend/can.hpp>
#include <modm/debug/logger.hpp>

#include <modm/driver/can/mcp2515.hpp>

using namespace modm::platform;
using systemClock = SystemClock;

// set new log level
#undef MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

#include "component_receiver/receiver.hpp"

#include "communication/postman.hpp"
#include "communication/identifier.hpp"

// ----------------------------------------------------------------------------
// Logging

modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull > loggerDevice;

modm::log::Logger modm::log::debug(loggerDevice);
modm::log::Logger modm::log::info(loggerDevice);
modm::log::Logger modm::log::warning(loggerDevice);
modm::log::Logger modm::log::error(loggerDevice);

// ----------------------------------------------------------------------------
// CAN communication

typedef GpioOutputB4 Cs;
typedef GpioInputB2 Int;

typedef GpioOutputB7 Sclk;
typedef GpioOutputB5 Mosi;
typedef GpioInputB6 Miso;

typedef BitBangSpiMaster< Sclk, Mosi, Miso > SPI;
typedef modm::Mcp2515< SPI, Cs, Int > CanDevice;

static CanDevice device;
static xpcc::CanConnector< CanDevice > connector(&device);

// create an instance of the generated postman
Postman postman;

xpcc::Dispatcher dispatcher(&connector, &postman);

// Default filters to receive any extended CAN frame
FLASH_STORAGE(uint8_t canFilter[]) =
{
	MCP2515_FILTER_EXTENDED(0),	// Filter 0
	MCP2515_FILTER_EXTENDED(0),	// Filter 1

	MCP2515_FILTER_EXTENDED(0),	// Filter 2
	MCP2515_FILTER_EXTENDED(0),	// Filter 3
	MCP2515_FILTER_EXTENDED(0),	// Filter 4
	MCP2515_FILTER_EXTENDED(0),	// Filter 5

	MCP2515_FILTER_EXTENDED(0),	// Mask 0
	MCP2515_FILTER_EXTENDED(0),	// Mask 1
};

namespace component
{
	Receiver receiver(robot::component::RECEIVER, dispatcher);
}

// ----------------------------------------------------------------------------
int
main()
{
	Uart0::connect<GpioOutputD1::Txd>();
	Uart0::initialize<systemClock, 115200>();

	// Initialize SPI interface and the other pins
	// needed by the MCP2515
	SPI::initialize<systemClock, 1000000>();
	Cs::setOutput();
	Int::setInput(Gpio::InputType::PullUp);

	// Configure MCP2515 and set the filters
	// Fixme: modm::Can::Bitrate is incompatitlbe with device driver
//    device.initialize(modm::can::BitRate::kBps125);
	device.setFilter(modm::accessor::asFlash(canFilter));

	// Enable Interrupts
	enableInterrupts();

	MODM_LOG_INFO << "Welcome to the communication test!" << modm::endl;

	while (1)
	{
		// deliver received messages
		dispatcher.update();

		component::receiver.update();

		modm::delayMicroseconds(100);
	}
}
