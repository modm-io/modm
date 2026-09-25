/*
 * Copyright (c) 2026, Luca Feggi
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <algorithm>
#include <array>
#include <modm/board.hpp>
#include <modm/platform/eth/eth.hpp>

using namespace modm::platform;

using Storage = EthernetStorage<4, 4>;
using Ethernet = Eth<Storage, modm::ethernet::MediaInterface::Rmii>;

Storage storage;

int
main()
{
	Board::initialize();
	Board::Leds::setOutput();

	Ethernet::connect<GpioC1::Mdc, GpioA2::Mdio, GpioA1::Refclk, GpioA7::Rcccrsdv, GpioC4::Rxd0,
					  GpioC5::Rxd1, GpioB11::Txen, GpioB12::Txd0, GpioB13::Txd1>();

	constexpr EthernetMacAddress MacAddress{0x02, 0, 0, 0, 0, 1};
	if (Ethernet::initialize<Board::SystemClock>(storage, MacAddress) != EthernetInitResult::Ok)
	{
		Board::Leds::set();
		for (;;) {}
	}

	if (Ethernet::setInternalLoopback(true) != EthernetConfigResult::Ok ||
		Ethernet::setLinkState({
			.up = true,
			.speed = modm::ethernet::LinkSpeed::Mbit100,
			.duplex = modm::ethernet::DuplexMode::Full,
			.autoNegotiated = false,
		}) != EthernetConfigResult::Ok)
	{
		Board::Leds::set();
		for (;;) {}
	}
	Ethernet::start();

	std::array<uint8_t, 64> frame{};
	std::copy(MacAddress.begin(), MacAddress.end(), frame.begin());
	std::copy(MacAddress.begin(), MacAddress.end(), frame.begin() + MacAddress.size());
	frame[12] = 0x88;
	frame[13] = 0xb5;
	(void)Ethernet::tryTransmit(frame);

	for (;;)
	{
		(void)Ethernet::poll();
		if (auto received = Ethernet::receive())
		{
			// The span is valid until the move-only lease is destroyed.
			if (!received->data().empty()) Board::Leds::toggle();
		}
	}
}
