/*
 * Copyright (c) 2020, Mike Wolfram
 * Copyright (c) 2021, Raphael Lehmann
 * Copyright (c) 2022, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/io.hpp>

// Create an IODeviceWrapper around the UsbCDC
modm::IODeviceWrapper<UsbUart0, modm::IOBuffer::DiscardIfFull> usb_io_device0;
modm::IOStream usb_stream0(usb_io_device0);

// connect Pin3 to Pin4 for loopback
using Mosi = GpioOutput3;
using Miso = GpioInput4;
using Sck = GpioOutput2;
using DmaRx = Dma::Channel1;
using DmaTx = Dma::Channel2;
using Spi = SpiMaster0_Dma<DmaRx, DmaTx>;

int main()
{
	Board::initialize();
	Board::initializeUsbFs();
	tusb_init();

	Spi::connect<Mosi::Tx, Miso::Rx, Sck::Sclk>();
	Spi::initialize<Board::SystemClock, 12_MHz>();

	while (true)
	{
		uint8_t sendBuffer[13] { "data to send" };
		uint8_t receiveBuffer[13];
		usb_stream0 << "Spi tx only" << modm::endl;
		tud_task();
		// send out 12 bytes, don't care about response
		Spi::transferBlocking(sendBuffer, nullptr, 12);

		usb_stream0 << "Spi send \"" << reinterpret_cast<const char*>(sendBuffer) << "\"" << modm::endl;
		tud_task();
		// send out 12 bytes, read in 12 bytes
		Spi::transferBlocking(sendBuffer, receiveBuffer, 12);
		receiveBuffer[12] = 0;
		usb_stream0 << "Spi received \"" << reinterpret_cast<const char*>(receiveBuffer) << "\"" << modm::endl;
		tud_task();
	}

	return 0;
}
