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

#include <cstddef>
#include <cstdint>

namespace modm::ethernet
{

/// Maximum frame passed between the MAC and application, including one VLAN tag but excluding FCS.
inline constexpr std::size_t MaximumFrameSize = 1518;

enum class MediaInterface : uint8_t
{
	Mii,
	Rmii,
};

enum class LinkSpeed : uint8_t
{
	Mbit10,
	Mbit100,
};

enum class DuplexMode : uint8_t
{
	Half,
	Full,
};

enum class MdioStatus : uint8_t
{
	Ok,
	Busy,
	Timeout,
	InvalidAddress,
};

struct LinkState
{
	bool up{false};
	LinkSpeed speed{LinkSpeed::Mbit10};
	DuplexMode duplex{DuplexMode::Half};
	bool autoNegotiated{false};

	constexpr bool
	operator==(const LinkState&) const = default;
};

enum class TransmitResult : uint8_t
{
	Ok,
	Busy,
	FrameTooLarge,
	InvalidLength,
	NotStarted,
	LinkDown,
	DmaError,
};

}  // namespace modm::ethernet
