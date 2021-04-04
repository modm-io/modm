/*
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_CAN_FILTER_HPP
#define MODM_CAN_FILTER_HPP

#include <modm/io/iostream.hpp>

namespace modm::can
{

/// CAN 2.0A 11 bit standard identifier
/// The upper 5 bits are ignored
/// @ingroup modm_architecture_can
struct StandardIdentifier
{
	uint16_t id;

	constexpr explicit operator uint16_t()
	{
		return id;
	}
};

/// CAN 2.0A 11 bit standard mask
/// The upper 5 bits are ignored
/// @ingroup modm_architecture_can
struct StandardMask
{
	uint16_t mask;

	constexpr explicit operator uint16_t()
	{
		return mask;
	}
};

/// CAN 2.0B 29 bit extended identifier
/// The upper 3 bits are ignored
/// @ingroup modm_architecture_can
struct ExtendedIdentifier
{
	uint32_t id;

	constexpr explicit operator uint32_t()
	{
		return id;
	}
};

/// CAN 2.0B 29 bit extended mask
/// The upper 3 bits are ignored
/// @ingroup modm_architecture_can
struct ExtendedMask
{
	uint32_t mask;

	constexpr explicit operator uint32_t()
	{
		return mask;
	}
};

}

#endif
