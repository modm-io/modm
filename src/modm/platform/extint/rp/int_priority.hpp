/*
 * Copyright (c) 2022, Nikolay Semenov
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

namespace modm::platform
{

/**
 * Priority level of 0-192 in steps of 64 for each interrupt.
 * A higher level corresponds to a lower priority,
 * so level 0 is the highest programmable interrupt priority.
 * ...
 * The processor implements only bits[7:6] of each field, bits [5:0] read as zero and ignore writes.
 * This means writing 255 to a priority register saves value 192 to the register.
 *
 * https://developer.arm.com/documentation/dui0662/b/Cortex-M0--Peripherals/Nested-Vectored-Interrupt-Controller
 * https://developer.arm.com/documentation/dui0662/b/Cortex-M0--Peripherals/Nested-Vectored-Interrupt-Controller/Interrupt-Priority-Registers
 *
 * @ingroup modm_platform_extint
 */
enum IntPriority : uint8_t
{
	Highest = 0x00,
	Default = 0x80,
	Lowest = 0xff,
};

}  // namespace modm::platform