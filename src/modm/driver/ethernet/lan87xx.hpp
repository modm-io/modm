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

#pragma once

#include <cstdint>

#include "phy.hpp"

namespace modm::ethernet
{

namespace lan87xx
{
inline constexpr uint8_t BasicControl = 0;
inline constexpr uint8_t BasicStatus = 1;
inline constexpr uint8_t Identifier1 = 2;
inline constexpr uint8_t Identifier2 = 3;
inline constexpr uint8_t Advertisement = 4;
inline constexpr uint8_t SpecialControlStatus = 31;

inline constexpr uint16_t SoftwareReset = 1u << 15;
inline constexpr uint16_t Speed100 = 1u << 13;
inline constexpr uint16_t AutoNegotiationEnable = 1u << 12;
inline constexpr uint16_t PowerDown = 1u << 11;
inline constexpr uint16_t Isolate = 1u << 10;
inline constexpr uint16_t RestartAutoNegotiation = 1u << 9;
inline constexpr uint16_t FullDuplex = 1u << 8;

inline constexpr uint16_t AutoNegotiationComplete = 1u << 5;
inline constexpr uint16_t LinkUp = 1u << 2;

inline constexpr uint16_t AutoNegotiationDone = 1u << 12;
inline constexpr uint16_t SpeedMask = 0x1cu;
inline constexpr uint16_t Speed10Half = 0x04u;
inline constexpr uint16_t Speed100Half = 0x08u;
inline constexpr uint16_t Speed10Full = 0x14u;
inline constexpr uint16_t Speed100Full = 0x18u;

inline constexpr uint16_t AdvertisementSelector8023 = 0x0001u;
}  // namespace lan87xx

template<Clause22Mdio Mdio, typename Traits, uint8_t Address = 0>
class Lan87xxPhy
{
	static_assert(Address < 32, "Clause-22 PHY addresses are five bits wide");

public:
	static constexpr uint8_t PhyAddress = Address;

	PhyResult
	readIdentifier(PhyIdentifier& identifier)
	{
		if (!read(lan87xx::Identifier1, identifier.ouiHigh) ||
			!read(lan87xx::Identifier2, identifier.ouiModelRevision))
		{
			return PhyResult::MdioError;
		}
		return isExpected(identifier) ? PhyResult::Ok : PhyResult::WrongDevice;
	}

	static constexpr bool
	isExpected(const PhyIdentifier& identifier)
	{
		return identifier.ouiHigh == Traits::Identifier1 &&
			   (identifier.ouiModelRevision & Traits::Identifier2Mask) == Traits::Identifier2;
	}

	PhyResult
	startSoftwareReset(uint32_t nowMilliseconds, uint32_t timeoutMilliseconds = 500)
	{
		if (!write(lan87xx::BasicControl, lan87xx::SoftwareReset))
		{
			state_ = State::Error;
			return PhyResult::MdioError;
		}
		deadline_ = nowMilliseconds + timeoutMilliseconds;
		state_ = State::Resetting;
		return PhyResult::InProgress;
	}

	PhyResult
	pollSoftwareReset(uint32_t nowMilliseconds)
	{
		if (state_ != State::Resetting) return PhyResult::InvalidState;

		uint16_t control;
		if (!read(lan87xx::BasicControl, control)) return PhyResult::MdioError;
		if ((control & lan87xx::SoftwareReset) == 0)
		{
			state_ = State::Ready;
			return PhyResult::Ok;
		}
		if (expired(nowMilliseconds))
		{
			state_ = State::Error;
			return PhyResult::Timeout;
		}
		return PhyResult::InProgress;
	}

	PhyResult
	configureAdvertisement(PhyAdvertisement advertisement = PhyAdvertisement::AllBaseModes)
	{
		const uint16_t value =
			lan87xx::AdvertisementSelector8023 | static_cast<uint16_t>(advertisement);
		return write(lan87xx::Advertisement, value) ? PhyResult::Ok : PhyResult::MdioError;
	}

	PhyResult
	startAutoNegotiation(uint32_t nowMilliseconds, uint32_t timeoutMilliseconds = 5000)
	{
		uint16_t control;
		if (!read(lan87xx::BasicControl, control)) return PhyResult::MdioError;
		control &= ~(lan87xx::PowerDown | lan87xx::Isolate);
		control |= lan87xx::AutoNegotiationEnable | lan87xx::RestartAutoNegotiation;
		if (!write(lan87xx::BasicControl, control)) return PhyResult::MdioError;

		deadline_ = nowMilliseconds + timeoutMilliseconds;
		state_ = State::Negotiating;
		linkState_.up = false;
		return PhyResult::InProgress;
	}

	PhyResult
	configureForcedLink(LinkSpeed speed, DuplexMode duplex)
	{
		uint16_t control;
		if (!read(lan87xx::BasicControl, control)) return PhyResult::MdioError;
		control &= ~(lan87xx::Speed100 | lan87xx::AutoNegotiationEnable | lan87xx::PowerDown |
					 lan87xx::Isolate | lan87xx::RestartAutoNegotiation | lan87xx::FullDuplex);
		if (speed == LinkSpeed::Mbit100) control |= lan87xx::Speed100;
		if (duplex == DuplexMode::Full) control |= lan87xx::FullDuplex;
		if (!write(lan87xx::BasicControl, control)) return PhyResult::MdioError;

		state_ = State::Ready;
		linkState_ = {
			.up = false,
			.speed = speed,
			.duplex = duplex,
			.autoNegotiated = false,
		};
		return PhyResult::Ok;
	}

	PhyResult
	pollAutoNegotiation(uint32_t nowMilliseconds)
	{
		if (state_ != State::Negotiating) return PhyResult::InvalidState;

		uint16_t status;
		if (!readStatus(status)) return PhyResult::MdioError;
		if ((status & (lan87xx::LinkUp | lan87xx::AutoNegotiationComplete)) ==
			(lan87xx::LinkUp | lan87xx::AutoNegotiationComplete))
		{
			const PhyResult result = readNegotiatedLink();
			if (result == PhyResult::Ok) state_ = State::Ready;
			return result;
		}
		if (expired(nowMilliseconds))
		{
			state_ = State::Ready;
			linkState_.up = false;
			return PhyResult::Timeout;
		}
		return PhyResult::InProgress;
	}

	PhyResult
	readLinkState()
	{
		uint16_t status;
		if (!readStatus(status)) return PhyResult::MdioError;
		if ((status & lan87xx::LinkUp) == 0)
		{
			linkState_.up = false;
			return PhyResult::LinkDown;
		}

		uint16_t control;
		if (!read(lan87xx::BasicControl, control)) return PhyResult::MdioError;
		if ((control & lan87xx::AutoNegotiationEnable) != 0)
		{
			if ((status & lan87xx::AutoNegotiationComplete) == 0)
			{
				linkState_.up = false;
				return PhyResult::InProgress;
			}
			return readNegotiatedLink();
		}

		linkState_ = {
			.up = true,
			.speed = (control & lan87xx::Speed100) ? LinkSpeed::Mbit100 : LinkSpeed::Mbit10,
			.duplex = (control & lan87xx::FullDuplex) ? DuplexMode::Full : DuplexMode::Half,
			.autoNegotiated = false,
		};
		return PhyResult::Ok;
	}

	const LinkState&
	linkState() const
	{ return linkState_; }

private:
	enum class State : uint8_t
	{
		Idle,
		Resetting,
		Negotiating,
		Ready,
		Error,
	};

	static bool
	read(uint8_t reg, uint16_t& value)
	{ return Mdio::read(Address, reg, value) == MdioStatus::Ok; }

	static bool
	write(uint8_t reg, uint16_t value)
	{ return Mdio::write(Address, reg, value) == MdioStatus::Ok; }

	bool
	readStatus(uint16_t& value)
	{
		// BSR link status is latched low. The second read is current state.
		uint16_t latchedStatus;
		return read(lan87xx::BasicStatus, latchedStatus) && read(lan87xx::BasicStatus, value);
	}

	PhyResult
	readNegotiatedLink()
	{
		uint16_t specialStatus;
		if (!read(lan87xx::SpecialControlStatus, specialStatus)) return PhyResult::MdioError;
		if ((specialStatus & lan87xx::AutoNegotiationDone) == 0)
		{
			linkState_.up = false;
			return PhyResult::InProgress;
		}

		linkState_.up = true;
		linkState_.autoNegotiated = true;
		switch (specialStatus & lan87xx::SpeedMask)
		{
			case lan87xx::Speed10Half:
				linkState_.speed = LinkSpeed::Mbit10;
				linkState_.duplex = DuplexMode::Half;
				break;
			case lan87xx::Speed10Full:
				linkState_.speed = LinkSpeed::Mbit10;
				linkState_.duplex = DuplexMode::Full;
				break;
			case lan87xx::Speed100Half:
				linkState_.speed = LinkSpeed::Mbit100;
				linkState_.duplex = DuplexMode::Half;
				break;
			case lan87xx::Speed100Full:
				linkState_.speed = LinkSpeed::Mbit100;
				linkState_.duplex = DuplexMode::Full;
				break;
			default:
				linkState_.up = false;
				return PhyResult::InvalidState;
		}
		return PhyResult::Ok;
	}

	bool
	expired(uint32_t nowMilliseconds) const
	{ return static_cast<int32_t>(nowMilliseconds - deadline_) >= 0; }

private:
	State state_{State::Idle};
	uint32_t deadline_{0};
	LinkState linkState_{};
};

}  // namespace modm::ethernet
