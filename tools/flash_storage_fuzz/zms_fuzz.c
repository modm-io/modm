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

// Zephyr ZMS (4x 2 kB) on the emulated flash, key-value only
#define LIB_NAME "zms"
#include <zephyr/kvss/zms.h>
#include "fuzz.h"

#define SEC 2048
static int rd(const struct device *d, off_t a, void *b, size_t n) { (void)d; return emu_read(a, b, n); }
static int wr(const struct device *d, off_t a, const void *b, size_t n) { (void)d; return emu_write(a, b, n); }
static int er(const struct device *d, off_t a, size_t n) { (void)d; return emu_erase(a, n); }
static struct flash_parameters params = {8, {0}, 0xff};
static const struct flash_parameters *gp(const struct device *d) { (void)d; return &params; }
static int pi(const struct device *d, off_t o, struct flash_pages_info *i)
{ (void)d; i->start_offset = o - o % SEC; i->size = SEC; i->index = o / SEC; return 0; }
static const struct flash_driver_api api = {rd, wr, er, gp, pi};
static const struct device dev = {&api, NULL};

#include "model.h"

static struct zms_fs fs;

// IDs at the edges of the range, 0xffffffff is ZMS_HEAD_ID
static uint32_t key_id(int k) { return k < 14 ? (uint32_t)k : k == 14 ? 0x7fffffff : 0xfffffffe; }
static ssize_t kv_read(int k, uint8_t *v) { return zms_read(&fs, key_id(k), v, MAXV); }
static int kv_write(int k, const uint8_t *v, size_t n)
{
	const ssize_t e = zms_write(&fs, key_id(k), v, n);
	return e == -ENOSPC ? -ENOSPC : e < 0 ? -EIO : 0;
}
static int kv_delete(int k) { return zms_delete(&fs, key_id(k)) ? -EIO : 0; }
// The garbage collection compacts the values, everything fitting into one
// sector must fit (the model counts 32 bytes per value for ATE and alignment)
static size_t lib_capacity(void) { return SEC - 5 * fs.ate_size - (fs.ate_size > 16 ? 32 * KEYS : 0); }
static int log_append(void) { return 0; }

static int lib_mount(void)
{
	memset(&fs, 0, sizeof fs);
	params.write_block_size = model.unit;
	fs.flash_device = &dev;
	fs.sector_size = SEC;
	fs.sector_count = 4;
	int e = zms_mount(&fs);
	if (trace) printf("    mounted %d: ate_wra 0x%llx data_wra 0x%llx cycle %u\n", e,
					  (unsigned long long)fs.ate_wra, (unsigned long long)fs.data_wra, fs.sector_cycle);
	if (e) FAIL("mount failed %d", e);
	return e;
}

static void lib_format(void)
{
	model.sector = SEC;
	model.size = 4 * SEC;
	model_reset();
	lib_mount();
}

static void lib_verify(void) { keys_verify(); }
static void lib_step(void) { model_step(0); }
