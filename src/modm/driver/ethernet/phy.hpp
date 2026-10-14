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

#include <concepts>
#include <cstdint>
#include <modm/architecture/interface/ethernet.hpp>

namespace modm::ethernet
{

template<typename Bus>
concept Clause22Mdio = requires(uint8_t phyAddress, uint8_t registerAddress, uint16_t value) {
	{ Bus::read(phyAddress, registerAddress, value) } -> std::same_as<MdioStatus>;
	{ Bus::write(phyAddress, registerAddress, value) } -> std::same_as<MdioStatus>;
};

enum class PhyResult : uint8_t
{
	Ok,
	InProgress,
	LinkDown,
	Timeout,
	MdioError,
	WrongDevice,
	InvalidState,
};

enum class PhyAdvertisement : uint16_t
{
	None = 0,
	Base10Half = 1u << 5,
	Base10Full = 1u << 6,
	Base100Half = 1u << 7,
	Base100Full = 1u << 8,
	Pause = 1u << 10,
	AsymmetricPause = 1u << 11,
	AllBaseModes = (1u << 5) | (1u << 6) | (1u << 7) | (1u << 8),
};

constexpr PhyAdvertisement
operator|(PhyAdvertisement left, PhyAdvertisement right)
{
	return static_cast<PhyAdvertisement>(static_cast<uint16_t>(left) |
										 static_cast<uint16_t>(right));
}

struct PhyIdentifier
{
	uint16_t ouiHigh{0};
	uint16_t ouiModelRevision{0};
};

}  // namespace modm::ethernet
