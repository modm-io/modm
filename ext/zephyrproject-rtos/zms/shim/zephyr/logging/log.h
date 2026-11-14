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

// The ZMS log goes to the modm logger up to CONFIG_ZMS_LOG_LEVEL:
// 0 = off, 1 = errors, 2 = warnings, 3 = info, 4 = debug
#pragma once
#ifndef CONFIG_ZMS_LOG_LEVEL
#define CONFIG_ZMS_LOG_LEVEL 0
#endif

#define LOG_MODULE_REGISTER(...)
void zms_log(int level, const char *fmt, ...);

#if CONFIG_ZMS_LOG_LEVEL >= 1
#define LOG_ERR(...) zms_log(1, __VA_ARGS__)
#else
#define LOG_ERR(...) ((void)0)
#endif
#if CONFIG_ZMS_LOG_LEVEL >= 2
#define LOG_WRN(...) zms_log(2, __VA_ARGS__)
#else
#define LOG_WRN(...) ((void)0)
#endif
#if CONFIG_ZMS_LOG_LEVEL >= 3
#define LOG_INF(...) zms_log(3, __VA_ARGS__)
#else
#define LOG_INF(...) ((void)0)
#endif
#if CONFIG_ZMS_LOG_LEVEL >= 4
#define LOG_DBG(...) zms_log(4, __VA_ARGS__)
#else
#define LOG_DBG(...) ((void)0)
#endif
