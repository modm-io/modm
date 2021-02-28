/*
 * Copyright (c) 2019, Niklas Hauser
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
#include <span>

namespace modm
{

/**
 * Return the GNU Build ID as a 160-bit SHA1 array.
 *
 * @ingroup modm_architecture_build_id
 */
[[nodiscard]]
std::span<const uint8_t, 20>
build_id();

}
