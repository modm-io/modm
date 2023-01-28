/*
 * Copyright (c) 2022, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <hardware/structs/resets.h>

#include <modm/architecture/interface/assert.hpp>
#include <modm/platform/device.hpp>

namespace modm::platform
{

/// @ingroup modm_platform_core
struct Resets
{
	static inline void
	reset(uint32_t bits)
	{
		hw_set_bits(&resets_hw->reset, bits);
	}
	static inline void
	unresetWait(uint32_t bits)
	{
		hw_clear_bits(&resets_hw->reset, bits);
		while (~resets_hw->reset_done & bits) __NOP();
	}
};

}  // namespace modm::platform
