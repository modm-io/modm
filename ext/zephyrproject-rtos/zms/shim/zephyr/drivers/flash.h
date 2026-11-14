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

// The parts of the Zephyr flash driver API that ZMS uses
#pragma once
#include <zephyr/kernel.h>
#include <zephyr/device.h>

#define FLASH_ERASE_C_EXPLICIT 0x01

struct flash_parameters
{
	size_t write_block_size;
	struct { uint32_t no_explicit_erase: 1; } caps;
	uint8_t erase_value;
};

struct flash_pages_info
{
	off_t start_offset;
	size_t size;
	uint32_t index;
};

/// Functions return 0 on success or a negative errno
struct flash_driver_api
{
	int (*read)(const struct device *dev, off_t offset, void *data, size_t len);
	int (*write)(const struct device *dev, off_t offset, const void *data, size_t len);
	int (*erase)(const struct device *dev, off_t offset, size_t size);
	const struct flash_parameters *(*get_parameters)(const struct device *dev);
	int (*get_page_info)(const struct device *dev, off_t offset, struct flash_pages_info *info);
};

static inline int flash_read(const struct device *dev, off_t offset, void *data, size_t len)
{ return dev->api->read(dev, offset, data, len); }
static inline int flash_write(const struct device *dev, off_t offset, const void *data, size_t len)
{ return dev->api->write(dev, offset, data, len); }
static inline int flash_erase(const struct device *dev, off_t offset, size_t size)
{ return dev->api->erase(dev, offset, size); }
static inline const struct flash_parameters *flash_get_parameters(const struct device *dev)
{ return dev->api->get_parameters(dev); }
static inline int flash_get_page_info_by_offs(const struct device *dev, off_t offset, struct flash_pages_info *info)
{ return dev->api->get_page_info(dev, offset, info); }
static inline int flash_params_get_erase_cap(const struct flash_parameters *p)
{ return p->caps.no_explicit_erase ? 0 : FLASH_ERASE_C_EXPLICIT; }
static inline bool flash_params_erase_value_readable(const struct flash_parameters *p)
{ (void)p; return true; }
