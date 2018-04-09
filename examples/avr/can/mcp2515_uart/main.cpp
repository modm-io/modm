/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012-2018, Niklas Hauser
 * Copyright (c) 2014, 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/platform.hpp>
#include <modm/architecture/interface/interrupt.hpp>

#include <modm/driver/can/mcp2515.hpp>
#include <modm/processing/timer.hpp>

using namespace modm::platform;
using systemClock = SystemClock;

typedef GpioOutputB0 LedGreen;
typedef GpioOutputB1 LedRed;

typedef GpioOutputB4 Cs;
typedef GpioInputB2 Int;

typedef GpioOutputB7 Sclk;
typedef GpioOutputB5 Mosi;
typedef GpioInputB6 Miso;

typedef BitBangSpiMaster<Sclk, Mosi, Miso> SPI;

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

// timer interrupt routine
MODM_ISR(TIMER2_COMPA)
{
	modm::Clock::increment();
}

int
main()
{
	LedGreen::setOutput(modm::Gpio::High);
	LedRed::setOutput(modm::Gpio::Low);

	// timer initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;

	Uart0::connect<GpioD1::Txd, GpioD0::Rxd>();
	Uart0::initialize<systemClock, 115200>();

	// Create a IOStream for complex formatting tasks
	modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull > device;
	modm::IOStream stream(device);

	// enable interrupts
	enableInterrupts();

	stream << "CAN Demo" << modm::endl;

	// Initialize SPI interface and the other pins
	// needed by the MCP2515
	SPI::connect<Sclk::BitBang, Mosi::BitBang, Miso::BitBang>();
	SPI::initialize<systemClock, 1000000>();
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

	modm::ShortPeriodicTimer timer(200);
	while (1)
	{
		if (mcp2515.isMessageAvailable())
		{
			modm::can::Message message;
			if (mcp2515.getMessage(message))
			{
				LedRed::toggle();

				stream << "Message received:" << modm::endl;
				stream.printf(" id   = %lx", message.getIdentifier());
				if (message.isExtended()) {
					stream << " extended";
				}
				else {
					stream << " standard";
				}
				if (message.isRemoteTransmitRequest()) {
					stream << ", rtr";
				}
				stream << modm::endl;

				stream << " dlc  = " << message.getLength() << modm::endl;
				if (!message.isRemoteTransmitRequest())
				{
					stream << " data = ";
					for (uint32_t i = 0; i < message.getLength(); ++i) {
						stream << modm::hex << message.data[i] << modm::ascii << ' ';
					}
					stream << modm::endl;
				}
			}
		}

		if (timer.execute())
		{
			LedGreen::toggle();
		}
	}
}
