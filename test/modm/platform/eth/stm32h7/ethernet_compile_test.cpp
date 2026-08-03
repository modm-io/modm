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

#include <modm/board.hpp>
#include <modm/driver/ethernet/lan8742a.hpp>
#include <modm/platform.hpp>

#include <concepts>

namespace
{

using Mac = modm::platform::EthernetMac;
using Phy = modm::Lan8742a<7>;
using DriverState = modm::platform::detail::EthH7MacState::DriverState;

static_assert(modm::ethernet::Clause22Mdio<Mac>);
static_assert(requires {
	{ Phy::initialize<Mac>() } -> std::same_as<Phy::InitializationResult>;
	{ Phy::readLinkStatus<Mac>() } -> std::same_as<Phy::LinkStatusResult>;
});
static_assert(Mac::LinkUpdateError::OutstandingLease != Mac::LinkUpdateError::None);

constexpr bool
isPhyMode(uint16_t hcd, Mac::Speed speed, Mac::DuplexMode duplex)
{
	const auto mode = Phy::decodeSpeedIndication(hcd);
	return mode and mode->speed == speed and mode->duplex == duplex;
}

static_assert(isPhyMode(Phy::SpeedIndication_t(Phy::SpeedIndication::Base10THalfDuplex).value,
		Mac::Speed::Speed10M, Mac::DuplexMode::Half));
static_assert(isPhyMode(Phy::SpeedIndication_t(Phy::SpeedIndication::Base10TFullDuplex).value,
		Mac::Speed::Speed10M, Mac::DuplexMode::Full));
static_assert(isPhyMode(Phy::SpeedIndication_t(Phy::SpeedIndication::Base100TxHalfDuplex).value,
		Mac::Speed::Speed100M, Mac::DuplexMode::Half));
static_assert(isPhyMode(Phy::SpeedIndication_t(Phy::SpeedIndication::Base100TxFullDuplex).value,
		Mac::Speed::Speed100M, Mac::DuplexMode::Full));
static_assert(not Phy::decodeSpeedIndication(0x00));
static_assert(not Phy::decodeSpeedIndication(0x0c));
static_assert(not Phy::decodeSpeedIndication(0x10));
static_assert(not Phy::decodeSpeedIndication(0x1c));
static_assert(uint16_t(Phy::BasicControl::SoftReset) == modm::Bit15);
static_assert(uint16_t(Phy::BasicControl::RestartAutoNegotiation) == modm::Bit9);
static_assert(uint16_t(Phy::BasicStatus::AutoNegotiationComplete) == modm::Bit5);
static_assert(uint16_t(Phy::AutoNegotiationAdvertisement::Base10THalfDuplex) ==
		modm::Bit5);
static_assert(uint16_t(Phy::AutoNegotiationAdvertisement::Base100TxHalfDuplex) ==
		modm::Bit7);
static_assert(uint16_t(Phy::AutoNegotiationAdvertisement::SymmetricPause) == modm::Bit10);
static_assert(uint16_t(Phy::AutoNegotiationAdvertisement::AsymmetricPause) == modm::Bit11);

static_assert(modm::platform::detail::needsRmii10MWorkaround(
		Mac::MediaInterface::RMII, Mac::Speed::Speed10M));
static_assert(not modm::platform::detail::needsRmii10MWorkaround(
		Mac::MediaInterface::RMII, Mac::Speed::Speed100M));
static_assert(not modm::platform::detail::needsRmii10MWorkaround(
		Mac::MediaInterface::MII, Mac::Speed::Speed10M));
static_assert(not modm::platform::detail::needsRmii10MWorkaround(
		Mac::MediaInterface::MII, Mac::Speed::Speed100M));

constexpr uint32_t ErrorSummary = modm::Bit15;
constexpr uint32_t DribbleError = modm::Bit19;
constexpr uint32_t ReceiveError = modm::Bit20;
constexpr uint32_t OverflowError = modm::Bit21;
constexpr uint32_t ReceiveWatchdog = modm::Bit22;
constexpr uint32_t GiantPacket = modm::Bit23;
constexpr uint32_t CrcError = modm::Bit24;
constexpr uint32_t FirstAndLastDescriptor = modm::Bit29 | modm::Bit28;
static_assert(modm::platform::detail::isExactRmii10MDribbleCrcError(
		ErrorSummary | DribbleError | CrcError));
static_assert(modm::platform::detail::isExactRmii10MDribbleCrcError(
		ErrorSummary | DribbleError | CrcError | FirstAndLastDescriptor));
static_assert(not modm::platform::detail::isExactRmii10MDribbleCrcError(
		DribbleError | CrcError));
static_assert(not modm::platform::detail::isExactRmii10MDribbleCrcError(
		ErrorSummary | CrcError));
static_assert(not modm::platform::detail::isExactRmii10MDribbleCrcError(
		ErrorSummary | DribbleError));
static_assert(not modm::platform::detail::isExactRmii10MDribbleCrcError(
		ErrorSummary | DribbleError | CrcError | ReceiveError));
static_assert(not modm::platform::detail::isExactRmii10MDribbleCrcError(
		ErrorSummary | DribbleError | CrcError | OverflowError));
static_assert(not modm::platform::detail::isExactRmii10MDribbleCrcError(
		ErrorSummary | DribbleError | CrcError | ReceiveWatchdog));
static_assert(not modm::platform::detail::isExactRmii10MDribbleCrcError(
		ErrorSummary | DribbleError | CrcError | GiantPacket));

static_assert(modm::platform::detail::canCommitLinkUpState(DriverState::Uninitialized));
static_assert(modm::platform::detail::canCommitLinkUpState(DriverState::Ready));
static_assert(modm::platform::detail::canCommitLinkUpState(DriverState::Running));
static_assert(not modm::platform::detail::canCommitLinkUpState(DriverState::Stopping));
static_assert(not modm::platform::detail::canCommitLinkUpState(DriverState::Faulted));

static_assert(modm::platform::detail::canFlushTransmitQueue(true, true, true));
static_assert(not modm::platform::detail::canFlushTransmitQueue(false, true, true));
static_assert(not modm::platform::detail::canFlushTransmitQueue(true, false, true));
static_assert(not modm::platform::detail::canFlushTransmitQueue(true, true, false));
static_assert(not modm::platform::detail::canFlushTransmitQueue(false, false, true));
static_assert(not modm::platform::detail::canFlushTransmitQueue(false, true, false));
static_assert(not modm::platform::detail::canFlushTransmitQueue(true, false, false));
static_assert(not modm::platform::detail::canFlushTransmitQueue(false, false, false));

static_assert(modm::platform::detail::needsFullToHalfDuplexFlush(
		Mac::DuplexMode::Full, Mac::DuplexMode::Half));
static_assert(not modm::platform::detail::needsFullToHalfDuplexFlush(
		Mac::DuplexMode::Full, Mac::DuplexMode::Full));
static_assert(not modm::platform::detail::needsFullToHalfDuplexFlush(
		Mac::DuplexMode::Half, Mac::DuplexMode::Full));
static_assert(not modm::platform::detail::needsFullToHalfDuplexFlush(
		Mac::DuplexMode::Half, Mac::DuplexMode::Half));

[[maybe_unused]] Mac::InitializationResult
initializeMii()
{
	const Mac::Configuration configuration{
		.macAddress = {0x02, 0, 0, 0, 0, 0},
		.checksumMode = Mac::ChecksumMode::Software,
	};
	return Mac::initialize<Board::SystemClock, Mac::MediaInterface::MII>(configuration);
}

} // namespace
