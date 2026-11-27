/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "parameter.hpp"
#include <modm/ext/zms/zms.hpp>

namespace modm::canard
{

/// Stores the parameters of a DroneCAN or Cyphal node in a mounted ZMS file system,
/// with the parameter key as ZMS ID. Each value takes 8 bytes inside its
/// ZMS allocation table entry.
class ZmsStorage : public ParameterStorage
{
public:
	explicit ZmsStorage(zms_fs& fs) : fs{fs} {}

	bool
	read(uint32_t key, uint64_t& data) override
	{
		return zms_read(&fs, key, &data, sizeof(data)) == sizeof(data);
	}

	bool
	write(uint32_t key, uint64_t data) override
	{
		return zms_write(&fs, key, &data, sizeof(data)) >= 0;
	}

	bool
	remove(uint32_t key) override
	{
		return zms_delete(&fs, key) == 0;
	}

private:
	zms_fs& fs;
};

}	// namespace modm::canard
