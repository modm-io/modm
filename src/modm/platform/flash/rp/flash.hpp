/*
 * Copyright (c) 2026, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include <cstring>

#include "../device.hpp"

namespace modm::platform
{

/// @ingroup modm_platform_flash
class Flash
{
public:
	static constexpr size_t BlockSize = 0x10000;
	static constexpr size_t SectorSize = 0x1000;
	static constexpr size_t PageSize = 0x100;

	static void
	eraseBlocks(size_t startBlock, size_t count);
	static void
	eraseSectors(size_t startSertor, size_t count);

	static void
	programPages(size_t startPage, const void *data, size_t count);

	static uint64_t
	getUniqueId();

	static void
	doCmd(const void *txbuf, void *rxbuf, size_t count);

	static void
	flush();
};

}  // namespace modm::platform
