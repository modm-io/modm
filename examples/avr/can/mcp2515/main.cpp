/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2015, 2017, Niklas Hauser
 * Copyright (c) 2014, 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform.hpp>
#include <modm/driver/can/mcp2515.hpp>

using namespace modm::platform;
using systemClock = SystemClock;

typedef GpioOutputB4 Cs;
typedef GpioInputB2 Int;

typedef GpioOutputB7 Sclk;
typedef GpioOutputB5 Mosi;
typedef GpioInputB6 Miso;

//typedef BitBangSpiMaster<Sclk, Mosi, Miso> SPI;
typedef SpiMaster SPI;

modm::Mcp2515<SPI, Cs, Int> mcp2515;

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

int
main()
{
	// Initialize SPI interface and the other pins
	// needed by the MCP2515
	SPI::connect<Sclk::Sck, Mosi::Mosi, Miso::Miso>();
	SPI::initialize<systemClock, 921600>();
	Cs::setOutput();
	Int::setInput(Gpio::InputType::PullUp);

	// Configure MCP2515 and set the filters
	mcp2515.initialize<modm::clock::MHz8, modm::Can::Bitrate::kBps125>();
	mcp2515.setFilter(modm::accessor::asFlash(canFilter));

	// Create a new message
	modm::can::Message message(0x123456);
	message.length = 2;
	message.data[0] = 0xab;
	message.data[1] = 0xcd;

	mcp2515.sendMessage(message);

	while (true)
	{
		if (mcp2515.isMessageAvailable())
		{
			modm::can::Message message;
			if (mcp2515.getMessage(message))
			{
				// do something
			}
		}
	}
}
