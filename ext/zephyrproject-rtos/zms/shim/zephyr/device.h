/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once
#include <stdbool.h>

struct flash_driver_api;
/// A flash device: the functions of the driver and their context
struct device
{
	const struct flash_driver_api *api;
	void *data;
};
static inline bool device_is_ready(const struct device *dev) { return dev && dev->api; }
