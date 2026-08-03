/*
 * Copyright (c) 2020, Mike Wolfram
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_LAN87XX_HPP
#define MODM_LAN87XX_HPP

#include <modm/architecture/interface/ethernet.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/processing/fiber.hpp>

#include <chrono>
#include <cstdint>
#include <optional>

namespace modm
{

/// Common Clause 22 register definitions and operations for LAN87xx PHYs.
struct Lan87xx
{
	struct Register
	{
		static constexpr uint8_t BasicControl = 0;
		static constexpr uint8_t BasicStatus = 1;
		static constexpr uint8_t PhyIdentifier1 = 2;
		static constexpr uint8_t PhyIdentifier2 = 3;
		static constexpr uint8_t AutoNegotiationAdvertisement = 4;
		static constexpr uint8_t AutoNegotiationLinkPartnerAbility = 5;
		static constexpr uint8_t InterruptSourceFlag = 29;
		static constexpr uint8_t PhySpecialControlStatus = 31;
	};

	enum class BasicControl : uint16_t
	{
		SoftReset = Bit15,
		Loopback = Bit14,
		SpeedSelect = Bit13,
		AutoNegotiationEnable = Bit12,
		PowerDown = Bit11,
		Isolate = Bit10,
		RestartAutoNegotiation = Bit9,
		DuplexMode = Bit8,
	};
	MODM_FLAGS16(BasicControl);

	enum class BasicStatus : uint16_t
	{
		AutoNegotiationComplete = Bit5,
		RemoteFault = Bit4,
		AutoNegotiationAbility = Bit3,
		LinkStatus = Bit2,
		JabberDetect = Bit1,
	};
	MODM_FLAGS16(BasicStatus);

	enum class AutoNegotiationAdvertisement : uint16_t
	{
		SelectorIeee8023 = Bit0,
		Base10THalfDuplex = Bit5,
		Base10TFullDuplex = Bit6,
		Base100TxHalfDuplex = Bit7,
		Base100TxFullDuplex = Bit8,
		// Pause capabilities require corresponding MAC flow control support
		SymmetricPause = Bit10,
		AsymmetricPause = Bit11,
		// Remote fault is status signalling, not a speed/duplex mode.
		RemoteFault = Bit13,
		AllSpeedDuplexModes = Base100TxFullDuplex | Base100TxHalfDuplex |
				Base10TFullDuplex | Base10THalfDuplex | SelectorIeee8023,
	};
	MODM_FLAGS16(AutoNegotiationAdvertisement);

	enum class PhySpecialControlStatus : uint16_t
	{
		AutoNegotiationDone = Bit12,
	};
	MODM_FLAGS16(PhySpecialControlStatus);

	enum class SpeedIndication : uint16_t
	{
		Base10THalfDuplex = 0b001,
		Base10TFullDuplex = 0b101,
		Base100TxHalfDuplex = 0b010,
		Base100TxFullDuplex = 0b110,
	};
	using SpeedIndication_t =
			modm::Configuration<PhySpecialControlStatus_t, SpeedIndication, 0b111, 2>;

	static constexpr auto SoftwareResetTimeout = std::chrono::milliseconds{550};

	enum class InitializationError : uint8_t
	{
		None,
		NotFound,
		ResetTimeout,
	};

	struct [[nodiscard]] InitializationResult
	{
		InitializationError error = InitializationError::None;
		ethernet::MdioError mdioError = ethernet::MdioError::None;

		constexpr explicit operator bool() const noexcept
		{
			return error == InitializationError::None and
					mdioError == ethernet::MdioError::None;
		}
	};

	struct [[nodiscard]] LinkStatusResult
	{
		ethernet::LinkStatus status;
		ethernet::MdioError error = ethernet::MdioError::None;

		constexpr explicit operator bool() const noexcept
		{
			return error == ethernet::MdioError::None;
		}
	};

	static constexpr std::optional<ethernet::LinkMode>
	decodeSpeedIndication(uint16_t specialStatus)
	{
		switch (SpeedIndication_t::get(PhySpecialControlStatus_t(specialStatus))) {
		case SpeedIndication::Base10THalfDuplex:
			return ethernet::LinkMode{
					ethernet::Speed::Speed10M, ethernet::DuplexMode::Half};
		case SpeedIndication::Base10TFullDuplex:
			return ethernet::LinkMode{
					ethernet::Speed::Speed10M, ethernet::DuplexMode::Full};
		case SpeedIndication::Base100TxHalfDuplex:
			return ethernet::LinkMode{
					ethernet::Speed::Speed100M, ethernet::DuplexMode::Half};
		case SpeedIndication::Base100TxFullDuplex:
			return ethernet::LinkMode{
					ethernet::Speed::Speed100M, ethernet::DuplexMode::Full};
		}
		return std::nullopt;
	}

protected:
	template <ethernet::Clause22Mdio Mdio, uint8_t Address>
	static InitializationResult
	initialize(uint16_t expectedIdentifier1, uint16_t expectedIdentifier2,
			uint16_t identifier2Mask)
	{
		static_assert(Address < 32, "Clause 22 PHY addresses are five bits wide");

		// Check the PHY is online and matches the expected device IDs
		uint16_t identifier1 = 0;
		uint16_t identifier2 = 0;
		if (const auto error = read<Mdio, Address>(Register::PhyIdentifier1, identifier1);
				error != ethernet::MdioError::None)
			return {.mdioError = error};
		if (const auto error = read<Mdio, Address>(Register::PhyIdentifier2, identifier2);
				error != ethernet::MdioError::None)
			return {.mdioError = error};
		if (identifier1 != expectedIdentifier1 or
				(identifier2 & identifier2Mask) != (expectedIdentifier2 & identifier2Mask))
			return {.error = InitializationError::NotFound};

		// Trigger a software reset and poll until it completes
		constexpr uint16_t SoftwareReset = uint16_t(BasicControl::SoftReset);
		if (const auto error = write<Mdio, Address>(Register::BasicControl, SoftwareReset);
				error != ethernet::MdioError::None)
			return {.mdioError = error};
		ethernet::MdioError pollError = ethernet::MdioError::None;
		const bool resetDone = modm::this_fiber::poll_for(SoftwareResetTimeout, [&] {
			uint16_t control = 0;
			pollError = read<Mdio, Address>(Register::BasicControl, control);
			return pollError != ethernet::MdioError::None or
					(control & SoftwareReset) == 0;
		});
		if (pollError != ethernet::MdioError::None)
			return {.mdioError = pollError};
		if (not resetDone)
			return {.error = InitializationError::ResetTimeout};

		// Advertise supported modes and restart auto negotiation
		if (const auto error = write<Mdio, Address>(Register::AutoNegotiationAdvertisement,
				uint16_t(AutoNegotiationAdvertisement::AllSpeedDuplexModes));
				error != ethernet::MdioError::None)
			return {.mdioError = error};
		if (const auto error = write<Mdio, Address>(Register::BasicControl,
				uint16_t(BasicControl::AutoNegotiationEnable) |
				uint16_t(BasicControl::RestartAutoNegotiation));
				error != ethernet::MdioError::None)
			return {.mdioError = error};
		return {};
	}

	template <ethernet::Clause22Mdio Mdio, uint8_t Address>
	static LinkStatusResult
	readLinkStatus()
	{
		uint16_t basicStatus = 0;
		// BMSR link status is latched low, so use the second read as current state.
		if (const auto error = read<Mdio, Address>(Register::BasicStatus, basicStatus);
				error != ethernet::MdioError::None)
			return {{}, error};
		if (const auto error = read<Mdio, Address>(Register::BasicStatus, basicStatus);
				error != ethernet::MdioError::None)
			return {{}, error};
		const BasicStatus_t status{basicStatus};
		if (status.none(BasicStatus::LinkStatus))
			return {{ethernet::LinkState::Down, std::nullopt}, {}};

		uint16_t basicControl = 0;
		if (const auto error = read<Mdio, Address>(Register::BasicControl, basicControl);
				error != ethernet::MdioError::None)
			return {{}, error};
		const BasicControl_t control{basicControl};
		if (control.none(BasicControl::AutoNegotiationEnable)) {
			return {{ethernet::LinkState::Up, ethernet::LinkMode{
					control.any(BasicControl::SpeedSelect) ?
							ethernet::Speed::Speed100M : ethernet::Speed::Speed10M,
					control.any(BasicControl::DuplexMode) ?
							ethernet::DuplexMode::Full : ethernet::DuplexMode::Half}}, {}};
		}
		if (status.none(BasicStatus::AutoNegotiationComplete))
			return {{ethernet::LinkState::Negotiating, std::nullopt}, {}};

		uint16_t specialStatus = 0;
		if (const auto error = read<Mdio, Address>(Register::PhySpecialControlStatus,
				specialStatus); error != ethernet::MdioError::None)
			return {{}, error};
		if (PhySpecialControlStatus_t(specialStatus).none(
				PhySpecialControlStatus::AutoNegotiationDone))
			return {{ethernet::LinkState::Negotiating, std::nullopt}, {}};

		const auto mode = decodeSpeedIndication(specialStatus);
		if (not mode)
			return {{ethernet::LinkState::Negotiating, std::nullopt}, {}};
		return {{ethernet::LinkState::Up, *mode}, {}};
	}

	template <ethernet::Clause22Mdio Mdio, uint8_t Address>
	static ethernet::MdioError
	read(uint8_t reg, uint16_t& value)
	{
		return Mdio::readPhyRegister(Address, reg, value);
	}

	template <ethernet::Clause22Mdio Mdio, uint8_t Address>
	static ethernet::MdioError
	write(uint8_t reg, uint16_t value)
	{
		return Mdio::writePhyRegister(Address, reg, value);
	}
};

} // namespace modm

#endif // MODM_LAN87XX_HPP
