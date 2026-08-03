/*
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// Sends raw IEEE 802 local experimental frames without a network stack.

#include <modm/board.hpp>
#include <modm/driver/ethernet/lan8742a.hpp>
#include <modm/platform/id/id.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

using namespace Board;
using namespace std::chrono_literals;

namespace
{

using Mac = modm::platform::EthernetMac;
using Phy = modm::Lan8742a<Board::eth::PhyAddress>;

constexpr std::size_t FrameSize = 60;
constexpr uint16_t ExperimentalEtherType = 0x88b5;
constexpr std::array<uint8_t, 16> PayloadMarker{{
	'm', 'o', 'd', 'm', '-', 's', 't', 'm', '3', '2', 'h', '7', '-', 'e', 't', 'h'
}};

void
logMacAddress(const modm::ethernet::MacAddress& address)
{
	MODM_LOG_INFO << "MAC " << modm::hex;
	for (std::size_t index = 0; index < address.size(); ++index) {
		MODM_LOG_INFO << address[index];
		if (index + 1 < address.size()) {
			MODM_LOG_INFO << ':';
		}
	}
	MODM_LOG_INFO << modm::ascii << modm::endl;
}

void
fillFrame(std::span<uint8_t> frame, const std::array<uint8_t, 6>& macAddress,
		uint32_t sequence)
{
	std::fill(frame.begin(), frame.end(), 0);
	std::fill_n(frame.begin(), 6, uint8_t{0xff});
	std::copy(macAddress.begin(), macAddress.end(), frame.begin() + 6);
	frame[12] = uint8_t(ExperimentalEtherType >> 8);
	frame[13] = uint8_t(ExperimentalEtherType);
	std::copy(PayloadMarker.begin(), PayloadMarker.end(), frame.begin() + 14);
	frame[30] = uint8_t(sequence);
	frame[31] = uint8_t(sequence >> 8);
	frame[32] = uint8_t(sequence >> 16);
	frame[33] = uint8_t(sequence >> 24);
}

const char*
linkStateName(modm::ethernet::LinkState state)
{
	switch (state)
	{
		case modm::ethernet::LinkState::Down: return "down";
		case modm::ethernet::LinkState::Negotiating: return "negotiating";
		case modm::ethernet::LinkState::Up: return "up";
	}
	return "unknown";
}

void
sendCopiedFrame(const std::array<uint8_t, 6>& macAddress, uint32_t sequence)
{
	std::array<uint8_t, FrameSize> frame{};
	fillFrame(frame, macAddress, sequence);
	const auto result = Mac::transmit(frame);
	modm_assert(result.error != Mac::TransmitError::Busy, "eth.tx.copy.busy",
			"Copied Ethernet transmit ring is busy");
	modm_assert(result, "eth.tx.copy", "Copied Ethernet transmit failed",
			static_cast<uintptr_t>(result.error));
	MODM_LOG_INFO << "TX copied sequence=" << sequence << modm::endl;
}

void
sendLeaseFrame(const std::array<uint8_t, 6>& macAddress, uint32_t sequence)
{
	auto lease = Mac::acquireTransmitBuffer(FrameSize);
	modm_assert(lease.error() != Mac::TransmitError::Busy, "eth.tx.acquire.busy",
			"Ethernet transmit ring is busy");
	modm_assert(lease, "eth.tx.acquire", "Failed to acquire Ethernet transmit buffer",
			static_cast<uintptr_t>(lease.error()));

	fillFrame(lease.buffer(), macAddress, sequence);
	const auto result = lease.commit();
	modm_assert(result, "eth.tx.commit", "Acquired Ethernet transmit commit failed",
			static_cast<uintptr_t>(result.error));
	MODM_LOG_INFO << "TX acquired sequence=" << sequence << modm::endl;
}

} // namespace

int
main()
{
	Board::initialize();
	MODM_LOG_INFO << "\n\nReboot: STM32H7 raw Ethernet MAC example" << modm::endl;

	Mac::connect<Board::eth::RefClk::Refclk,
		Board::eth::Mdc::Mdc,
		Board::eth::Mdio::Mdio,
		Board::eth::CrsDv::Rcccrsdv,
		Board::eth::TxEn::Txen,
		Board::eth::Txd0::Txd0,
		Board::eth::Txd1::Txd1,
		Board::eth::Rxd0::Rxd0,
		Board::eth::Rxd1::Rxd1>();

	const auto macAddress = modm::platform::getUniqueMacAddress();
	const Mac::Configuration configuration{
		.macAddress = macAddress,
		.checksumMode = Mac::ChecksumMode::Software,
	};
	const auto initialized =
			Mac::initialize<Board::SystemClock, Mac::MediaInterface::RMII>(configuration);
	modm_assert(initialized, "eth.initialize", "Ethernet MAC initialization failed",
			static_cast<uintptr_t>(initialized.error));
	const auto phyInitialized = Phy::initialize<Mac>();
	modm_assert(phyInitialized, "eth.phy.initialize", "Ethernet PHY initialization failed",
			static_cast<uintptr_t>(phyInitialized.error));

	logMacAddress(macAddress);
	MODM_LOG_INFO << "EtherType 0x88b5" << modm::endl;

	bool haveLinkState = false;
	modm::ethernet::LinkState previousLinkState = modm::ethernet::LinkState::Down;
	uint32_t sequence = 0;
	uint8_t serviceTicks = 0;
	while (true)
	{
		const auto observed = Phy::readLinkStatus<Mac>();
		modm_assert(observed, "eth.phy.link", "Ethernet PHY link read failed",
				static_cast<uintptr_t>(observed.error));
		const auto link = Mac::notifyUpdatedLinkStatus(observed.status);
		modm_assert(link, "eth.link", "Ethernet MAC link update failed",
				static_cast<uintptr_t>(link.error));
		if (not haveLinkState or link.status.state != previousLinkState) {
			previousLinkState = link.status.state;
			haveLinkState = true;
			MODM_LOG_INFO << "Link " << linkStateName(previousLinkState) << modm::endl;
		}

		if (++serviceTicks == 10) {
			serviceTicks = 0;
			if (previousLinkState == modm::ethernet::LinkState::Up) {
				if ((sequence & 1u) == 0) {
					sendCopiedFrame(macAddress, sequence);
				}
				else {
					sendLeaseFrame(macAddress, sequence);
				}
				sequence++;
			}
		}

		modm::delay(100ms);
	}
}
