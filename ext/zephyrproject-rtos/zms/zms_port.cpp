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
#include <modm/math/utils/crc.hpp>

extern "C" uint8_t
crc8_ccitt(uint8_t crc, const void *buf, size_t len)
{
	const auto *data = static_cast<const uint8_t *>(buf);
	while (len--) crc = modm::math::crc8_ccitt_update(crc, *data++);
	return crc;
}

extern "C" uint32_t
crc32_ieee(const uint8_t *data, size_t len)
{ return modm::math::crc32(data, len); }

extern "C" __attribute__((weak)) int k_mutex_init(k_mutex *) { return 0; }
extern "C" __attribute__((weak)) int k_mutex_lock(k_mutex *, k_timeout_t) { return 0; }
extern "C" __attribute__((weak)) int k_mutex_unlock(k_mutex *) { return 0; }
