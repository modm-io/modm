/*
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_ARCHITECTURE_ETHERNET_HPP
#define MODM_ARCHITECTURE_ETHERNET_HPP

#include <array>
#include <concepts>
#include <cstdint>
#include <optional>

namespace modm::ethernet
{

using MacAddress = std::array<uint8_t, 6>;

enum class MediaInterface : uint8_t
{
	MII,
	RMII,
};

enum class Speed : uint8_t
{
	Speed10M,
	Speed100M,
};

enum class DuplexMode : uint8_t
{
	Half,
	Full,
};

struct LinkMode
{
	Speed speed = Speed::Speed100M;
	DuplexMode duplex = DuplexMode::Full;

	friend bool operator==(LinkMode const&, LinkMode const&) = default;
};

enum class LinkState : uint8_t
{
	Down,
	Negotiating,
	Up,
};

struct LinkStatus
{
	LinkState state = LinkState::Down;
	std::optional<LinkMode> mode;
};

enum class ChecksumMode : uint8_t
{
	Software,
	Hardware,
};

enum class MdioError : uint8_t
{
	None,
	Busy,
	Timeout,
	InvalidRegister,
	InvalidPhyAddress,
};

template <class Controller>
concept Clause22Mdio = requires(uint8_t phyAddress, uint8_t reg,
		uint16_t value, uint16_t& result) {
	{ Controller::writePhyRegister(phyAddress, reg, value) } -> std::same_as<MdioError>;
	{ Controller::readPhyRegister(phyAddress, reg, result) } -> std::same_as<MdioError>;
};

} // namespace modm::ethernet

#endif // MODM_ARCHITECTURE_ETHERNET_HPP
