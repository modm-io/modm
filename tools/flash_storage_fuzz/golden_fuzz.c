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

// A trivially power-loss safe store to check the harness: every update erases
// the older of two sectors and writes all keys with a sequence number and a
// CRC into it. It must survive every model, otherwise the harness is wrong.
#define LIB_NAME "golden"
#include <stdint.h>
#include <sys/types.h>
#include "fuzz.h"

#define SEC 2048
#include "model.h"

struct record
{
	uint32_t sequence;
	uint32_t crc;
	uint8_t length[KEYS];  // 0 = deleted
	uint8_t value[KEYS][MAXV];
	uint8_t padding[32 - (8 + KEYS + KEYS * MAXV) % 32];
};
static struct record ram;
static uint32_t sector; // sector of the current record

static uint32_t crc(const struct record *r)
{
	uint32_t c = 0xffffffff;
	for (const uint8_t *p = (const uint8_t *)&r->length; p < (const uint8_t *)(r + 1); p++) {
		c ^= *p;
		for (int i = 0; i < 8; i++) c = (c >> 1) ^ (0xEDB88320u & -(c & 1));
	}
	return ~c ^ r->sequence;
}

static int store(void)
{
	ram.sequence++;
	ram.crc = crc(&ram);
	const uint32_t next = (sector + 1) % 2;
	if (emu_erase(next * SEC, SEC) || emu_write(next * SEC, &ram, sizeof ram)) return -EIO;
	sector = next;
	return 0;
}

static ssize_t kv_read(int k, uint8_t *v)
{
	if (!ram.length[k]) return -ENOENT;
	memcpy(v, ram.value[k], ram.length[k]);
	return ram.length[k];
}
static int kv_write(int k, const uint8_t *v, size_t n)
{
	ram.length[k] = n;
	memcpy(ram.value[k], v, n);
	return store();
}
static int kv_delete(int k) { ram.length[k] = 0; return store(); }
static int log_append(void) { return 0; }
static size_t lib_capacity(void) { return (size_t)-1; }

static int lib_mount(void)
{
	struct record r[2];
	int valid[2];
	for (int s = 0; s < 2; s++) {
		emu_read(s * SEC, &r[s], sizeof r[s]);
		valid[s] = r[s].crc == crc(&r[s]);
	}
	if (!valid[0] && !valid[1]) { FAIL("no valid record"); return -1; }
	sector = (valid[0] && (!valid[1] || (int32_t)(r[0].sequence - r[1].sequence) > 0)) ? 0 : 1;
	ram = r[sector];
	return 0;
}

static void lib_format(void)
{
	model.sector = SEC;
	model.size = 2 * SEC;
	model_reset();
	memset(&ram, 0, sizeof ram);
	sector = 1;
	store();
}

static void lib_verify(void) { keys_verify(); }
static void lib_step(void) { model_step(0); }
