/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013-2017, Niklas Hauser
 * Copyright (c) 2016, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/driver/can/mcp2515.hpp>

// Set the log level
#undef MODM_LOG_LEVEL
#define MODM_LOG_LEVEL modm::log::DEBUG

// If you use a different SPI instance, you may have to also choose different
// GPIOs to connect to.
using Cs = GpioOutputA4;
using Mosi = GpioOutputB5;
using Miso = GpioInputB4;
using Sck = GpioOutputB3;
using Int = GpioInputC7;
using SpiMaster = SpiMaster1;
// Note that you can also use a bit-banged SPI driver as a drop-in replacement
// using SpiMaster = BitBangSpiMaster<Sck, Mosi, Miso>;

// Default filters to receive any extended CAN frame
const uint8_t canFilter[] =
{
	MCP2515_FILTER_EXTENDED(0), // Filter 0
	MCP2515_FILTER_EXTENDED(0), // Filter 1

	MCP2515_FILTER(0), // Filter 2
	MCP2515_FILTER(0), // Filter 3
	MCP2515_FILTER(0), // Filter 4
	MCP2515_FILTER(0), // Filter 5

	MCP2515_MASK_EXTENDED(0), // Mask 0
	MCP2515_MASK(0), // Mask 1
};

modm::Mcp2515<SpiMaster, Cs, Int> mcp2515;

modm::Fiber fiber_mcp2515([]
{
	MODM_LOG_INFO << "Initializing mcp2515 ..." << modm::endl;
	mcp2515.initialize<8_MHz, 500_kbps>();

	while(1)
	{
		mcp2515.update();
		modm::this_fiber::yield();
	}
});

modm::Fiber fiber_can([]
{
	modm::can::Message msg;
	modm::ShortPeriodicTimer tmr{1s};
	uint8_t i, j;

	/// Set filters of MCP2515
	MODM_LOG_INFO << "Setting filters of mcp2515 ..." << modm::endl;
	mcp2515.setFilter(canFilter);
	MODM_LOG_INFO << "Running ... " << modm::endl;
	while (true)
	{
		// receive messages
		if (mcp2515.isMessageAvailable())
		{
			MODM_LOG_INFO << "Message Available ... " << modm::endl;
			mcp2515.getMessage(msg);
			MODM_LOG_INFO << "Received message: " << modm::hex << msg.identifier << modm::endl;
			for(i = 0; i < msg.length; ++i){
				MODM_LOG_INFO << modm::hex << " 0x" << msg.data[i];
			}
			MODM_LOG_INFO << modm::endl << modm::endl;
		}

		if(tmr.execute())
		{
			msg.identifier = 0xAA;
			msg.length = 2;
			msg.data[0] = 13;
			msg.data[1] = 37;
			MODM_LOG_INFO << "Sending Message ... "<< modm::endl;
			for(j = 0; j < msg.length; ++j){
				MODM_LOG_INFO << modm::hex<< " 0x" << msg.data[j];
			}
			MODM_LOG_INFO << modm::endl;
			MODM_LOG_INFO << "Success: " << mcp2515.sendMessage(msg) << modm::endl;
		}

		modm::this_fiber::yield();
	}
});

int
main()
{
	Board::initialize();
	SpiMaster::connect<Miso::Miso, Mosi::Mosi, Sck::Sck>();
	SpiMaster::initialize<Board::SystemClock, 10_MHz>();

	MODM_LOG_INFO << "Mcp2515 Example" << modm::endl;

	modm::fiber::Scheduler::run();
	return 0;
}
