/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

namespace modm::graphic
{
enum class Style : uint8_t
{
	Outline,
	Filled,
	// Outline_Dotted,
	// Outline_Dashed,
};
} // namespace modm::graphic