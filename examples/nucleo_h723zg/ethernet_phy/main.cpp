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

#include <modm/board.hpp>
#include <modm/driver/ethernet/lan8720a.hpp>
#include <modm/driver/ethernet/lan8742a.hpp>
#include <modm/platform/eth/eth.hpp>
#include <modm/processing/timer.hpp>

using namespace modm::platform;
using namespace modm::literals;

using Storage = EthernetStorage<4, 4>;
using Ethernet = Eth<Storage, modm::ethernet::MediaInterface::Rmii>;

// Both PHYs use the same non-blocking API. Select the fitted device and its
// strapped Clause-22 address here.
using Lan8720AtAddress0 = modm::Lan8720aPhy<Ethernet, 0>;
using Lan8742AtAddress0 = modm::Lan8742aPhy<Ethernet, 0>;
using Phy = Lan8742AtAddress0;

Storage storage;
Phy phy;

enum class PhyState : uint8_t
{
	Resetting,
	Negotiating,
	Running,
	Failed,
};

bool
applyPhyLinkState()
{
	const auto& state = phy.linkState();
	if (state == Ethernet::linkState()) return true;
	if (Ethernet::isStarted()) Ethernet::stop();
	if (Ethernet::setLinkState(state) != EthernetConfigResult::Ok) return false;
	if (state.up) Ethernet::start();
	return true;
}

int
main()
{
	Board::initialize();
	Board::Leds::setOutput();

	// If the board provides a PHY reset GPIO, assert and release it here with
	// the timing required by the selected PHY before accessing MDIO.
	Ethernet::connect<GpioC1::Mdc, GpioA2::Mdio, GpioA1::Refclk, GpioA7::Rcccrsdv, GpioC4::Rxd0,
					  GpioC5::Rxd1, GpioB11::Txen, GpioB12::Txd0, GpioB13::Txd1>();

	constexpr EthernetMacAddress MacAddress{0x02, 0, 0, 0, 0, 2};
	if (Ethernet::initialize<Board::SystemClock>(storage, MacAddress) != EthernetInitResult::Ok)
	{
		Board::Leds::set();
		for (;;) {}
	}

	modm::ethernet::PhyIdentifier identifier;
	if (phy.readIdentifier(identifier) != modm::ethernet::PhyResult::Ok)
	{
		Board::Leds::set();
		for (;;) {}
	}

	auto now = modm::Clock::now().time_since_epoch().count();
	PhyState phyState = PhyState::Resetting;
	if (phy.startSoftwareReset(now) != modm::ethernet::PhyResult::InProgress)
		phyState = PhyState::Failed;

	modm::PeriodicTimer phyTimer{10ms};
	for (;;)
	{
		(void)Ethernet::poll();
		if (auto frame = Ethernet::receive())
		{
			// Pass frame->data() to the network stack before the lease expires.
			Board::Leds::toggle();
		}

		if (!phyTimer.execute()) continue;
		now = modm::Clock::now().time_since_epoch().count();

		switch (phyState)
		{
			case PhyState::Resetting: {
				const auto result = phy.pollSoftwareReset(now);
				if (result == modm::ethernet::PhyResult::Ok &&
					phy.configureAdvertisement() == modm::ethernet::PhyResult::Ok &&
					phy.startAutoNegotiation(now) == modm::ethernet::PhyResult::InProgress)
				{
					phyState = PhyState::Negotiating;
				} else if (result != modm::ethernet::PhyResult::InProgress)
				{
					phyState = PhyState::Failed;
				}
				break;
			}

			case PhyState::Negotiating: {
				const auto result = phy.pollAutoNegotiation(now);
				if (result == modm::ethernet::PhyResult::Ok)
				{
					phyState = applyPhyLinkState() ? PhyState::Running : PhyState::Failed;
				} else if (result != modm::ethernet::PhyResult::InProgress)
				{
					phyState = PhyState::Failed;
				}
				break;
			}

			case PhyState::Running: {
				const auto result = phy.readLinkState();
				if (result == modm::ethernet::PhyResult::Ok ||
					result == modm::ethernet::PhyResult::LinkDown ||
					result == modm::ethernet::PhyResult::InProgress)
				{
					if (!applyPhyLinkState()) phyState = PhyState::Failed;
				} else
				{
					Ethernet::stop();
					phyState = PhyState::Failed;
				}
				break;
			}

			case PhyState::Failed:
				Board::Leds::set();
				break;
		}
	}
}
