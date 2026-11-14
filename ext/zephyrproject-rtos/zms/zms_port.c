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

// CRCs for ZMS, the mutex is only needed if several fibers share a ZMS
#include <zephyr/kernel.h>
#include <zephyr/sys/crc.h>

uint8_t
crc8_ccitt(uint8_t crc, const void *buf, size_t len)
{
	const uint8_t *data = buf;
	while (len--)
	{
		crc ^= *data++;
		for (int ii = 0; ii < 8; ii++)
			crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : crc << 1;
	}
	return crc;
}

uint32_t
crc32_ieee_update(uint32_t crc, const uint8_t *data, size_t len)
{
	crc = ~crc;
	while (len--)
	{
		crc ^= *data++;
		for (int ii = 0; ii < 8; ii++)
			crc = (crc >> 1) ^ (0xEDB88320u & -(crc & 1));
	}
	return ~crc;
}

uint32_t
crc32_ieee(const uint8_t *data, size_t len)
{ return crc32_ieee_update(0, data, len); }

__attribute__((weak)) int k_mutex_init(struct k_mutex *m) { (void)m; return 0; }
__attribute__((weak)) int k_mutex_lock(struct k_mutex *m, k_timeout_t t) { (void)m; (void)t; return 0; }
__attribute__((weak)) int k_mutex_unlock(struct k_mutex *m) { (void)m; return 0; }
