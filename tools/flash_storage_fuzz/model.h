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

// The reference model of a key-value store: 16 keys with values of 1-64 bytes,
// 1/8 of them ending with 0xFF bytes like -1 or erased structs. The library
// file implements kv_read/kv_write/kv_delete (and log_append for logs).
#pragma once

#define KEYS 16
#ifndef MAXV
#define MAXV 64  // longer values: 1/8 of them up to MAXV, the others up to 64 bytes
#endif
#define MAXL 128

static ssize_t kv_read(int key, uint8_t *value);   // length or -ENOENT
static int kv_write(int key, const uint8_t *value, size_t length);
static int kv_delete(int key);
static int log_append(void);
static size_t lib_capacity(void);              // value bytes that always fit

static uint32_t mix(uint32_t x) { x ^= x >> 16; x *= 0x7feb352d; x ^= x >> 15; x *= 0x846ca68b; x ^= x >> 16; return x; }
static size_t fill(uint32_t seed, uint8_t *b, size_t max)
{
	size_t n = 1 + mix(seed) % max;
	for (size_t i = 0; i < n; i++) b[i] = mix(seed ^ (i * 0x01000193));
	if (seed % 8 == 0) memset(b + n / 2, 0xff, n - n / 2);
	return n;
}
static size_t fill_value(uint32_t seed, uint8_t *b) { return fill(seed, b, (seed & 0x70) && MAXV > 64 ? 64 : MAXV); }

static uint32_t seeds[KEYS];   // 0 = deleted
static int pending_key = -1;   // the key of an interrupted update
static uint32_t pending_seed;
static unsigned long updates;
static unsigned long lib_updates(void) { return updates; }

static void model_reset(void) { memset(seeds, 0, sizeof seeds); pending_key = -1; updates = 0; }

static int matches(uint32_t seed, const uint8_t *v, ssize_t n)
{
	uint8_t e[MAXV];
	if (!seed) return n == -ENOENT;
	return n == (ssize_t)fill_value(seed, e) && !memcmp(v, e, n);
}

/// Only the interrupted key may have its old or its new value
static void key_verify(int k)
{
	uint8_t v[MAXV];
	ssize_t n = kv_read(k, v);
	if (k == pending_key) {
		const int is_new = matches(pending_seed, v, n);
		if (!is_new && !matches(seeds[k], v, n)) FAIL("key %d interrupted: neither old nor new (%zd)", k, n);
		if (is_new) seeds[k] = pending_seed;
		pending_key = -1;
	} else if (!matches(seeds[k], v, n)) FAIL("key %d: lost or wrong (%zd)", k, n);
}
static void keys_verify(void) { for (int k = 0; k < KEYS && !failure[0]; k++) key_verify(k); }

static void model_step(int with_logs)
{
	uint8_t v[MAXV];
	const int k = rnd(KEYS);
	const uint32_t op = rnd(10);
	if (trace) printf("  step key %d op %u\n", k, op);
	if (op < 6) {
		const uint32_t seed = rnd(0xfffffffe) + 1;
		const size_t n = fill_value(seed, v);
		pending_seed = seed; pending_key = k;
		const int e = kv_write(k, v, n);
		if (e == -ENOSPC) {
			// Only allowed when the values (plus 32 bytes each) fill the capacity
			size_t live = n + 32;
			for (int i = 0; i < KEYS; i++) if (seeds[i] && i != k) live += fill_value(seeds[i], v) + 32;
			if (live <= lib_capacity()) FAIL("key %d: no space with %zu bytes", k, live);
			pending_key = -1;
		} else {
			if (e) FAIL("key %d: write failed (%d)", k, e);
			seeds[k] = seed; pending_key = -1; updates++;
		}
	} else if (op < 7) {
		pending_seed = 0; pending_key = k;
		if (kv_delete(k)) FAIL("key %d: delete failed", k);
		seeds[k] = 0; pending_key = -1; updates++;
	}
	key_verify(k);
	if (with_logs && rnd(4) == 0 && log_append()) FAIL("log append failed");
}
