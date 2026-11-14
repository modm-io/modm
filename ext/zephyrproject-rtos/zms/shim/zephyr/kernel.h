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

// The parts of the Zephyr kernel API that ZMS uses
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <zms_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Implemented by the port, may be empty without concurrency
struct k_mutex { void *impl; };
typedef struct { int ticks; } k_timeout_t;
#define K_FOREVER ((k_timeout_t){-1})
int k_mutex_init(struct k_mutex *mutex);
int k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout);
int k_mutex_unlock(struct k_mutex *mutex);

#ifdef __cplusplus
}
#endif

#define BIT(n) (1UL << (n))
#define GENMASK(h, l) ((~0U << (l)) & (~0U >> (31 - (h))))
#define GENMASK64(h, l) (((~0ULL) - (1ULL << (l)) + 1) & (~0ULL >> (63 - (h))))
#define LSB_GET(v) ((v) & -(v))
#define FIELD_GET(m, v) (((v) & (m)) / LSB_GET(m))
#define FIELD_PREP(m, v) (((v) * LSB_GET(m)) & (m))
#define SIZEOF_FIELD(t, m) sizeof(((t *)0)->m)
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#define ARG_UNUSED(x) (void)(x)
#define BUILD_ASSERT(...) _Static_assert(__VA_ARGS__)
#define IS_ENABLED(x) (x)
#define __ASSERT(...)
#define __ASSERT_NO_MSG(...)
#ifndef __packed
#define __packed __attribute__((packed))
#endif
#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif
