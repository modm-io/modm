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
#include <modm/driver/can/mcp2515.hpp>
#include <modm/processing/protothread.hpp>

// If you use a different SPI instance, you may have to also choose different
// GPIOs to connect to.
using Int = GpioOutputB11;
using Cs = GpioOutputB12;
using Sck = GpioOutputB13;
using Mosi = GpioOutputB15;
using Miso = GpioInputB14;
using SpiMaster = SpiMaster2;
// Note that you can also use a bit-banged SPI driver as a drop-in replacement
// using SpiMaster = BitBangSpiMaster<Sck, Mosi, Miso>;

// Default filters to receive any extended CAN frame
FLASH_STORAGE(uint8_t canFilter[]) = {
	MCP2515_FILTER_EXTENDED(0),  // Filter 0
	MCP2515_FILTER_EXTENDED(0),  // Filter 1

	MCP2515_FILTER_EXTENDED(0),  // Filter 2
	MCP2515_FILTER_EXTENDED(0),  // Filter 3
	MCP2515_FILTER_EXTENDED(0),  // Filter 4
	MCP2515_FILTER_EXTENDED(0),  // Filter 5

	MCP2515_FILTER_EXTENDED(0),  // Mask 0
	MCP2515_FILTER_EXTENDED(0),  // Mask 1
};

modm::Mcp2515<SpiMaster, Cs, Int> mcp2515;

class MyTask : modm::pt::Protothread
{
public:
	MyTask() : message_{0x123456}{}

	bool
	run()
	{
		PT_BEGIN();

		// Configure MCP2515 and set the filters
		mcp2515.attachConfigurationHandler([]() {
			SpiMaster::setDataMode(SpiMaster::DataMode::Mode0);
			SpiMaster::setDataOrder(SpiMaster::DataOrder::MsbFirst);
			/// @chris-durand
			SpiMaster::setBaudrate<Board::SystemClock, 10_MHz>();
		});
		mcp2515.initialize<8_MHz, 125_kbps>();
		mcp2515.setFilter(modm::accessor::asFlash(canFilter));

		// send a new message
		message_.length = 2;
		message_.data[0] = 0xab;
		message_.data[1] = 0xcd;
		mcp2515.sendMessage(message_);

		while (true)
		{
			if (mcp2515.isMessageAvailable())
			{
				mcp2515.getMessage(message_);
				MODM_LOG_INFO << "Received message: " << message_.identifier << modm::endl;
			}
			PT_CALL(mcp2515.update());
		}
		PT_END();
	}

private:
	modm::can::Message message_;
};

MyTask task;

int
main()
{
	Board::initialize();

	// Initialize SPI interface and the other pins
	// needed by the MCP2515
	SpiMaster::connect<Miso::Miso, Mosi::Mosi, Sck::Sck>();
	/// we initialize a higher baud rate then n the avr example, dunnow hats the mcp2515 is capable
	/// of
	SpiMaster::initialize<Board::SystemClock, 20_MHz>();
	Cs::setOutput();
	Int::setInput(Gpio::InputType::PullUp);

	while (true) {
		task.run();
	}
}