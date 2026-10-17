/*
 * Copyright (c) 2024, Frank Altheim
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef LV_CONF_H
#	error	"Don't include this file directly, use 'lv_conf.h' instead!"
#endif

// Maximal resolutions
#define LV_HOR_RES_MAX  240
#define LV_VER_RES_MAX  320
#define LV_DPI          200

// Default color format: LV_COLOR_FORMAT_{I1,L8,RGB565,RGB888,XRGB8888,...}
#define LV_COLOR_FORMAT_DEFAULT  LV_COLOR_FORMAT_RGB565

// Enable logging at INFO level
#define LV_USE_LOG  1
#define LV_LOG_LEVEL  LV_LOG_LEVEL_INFO

// Fonts:
#define LV_FONT_MONTSERRAT_36  1
#define LV_FONT_MONTSERRAT_24  1
#define LV_FONT_MONTSERRAT_16  1

// Disable anti-aliasing
#define LV_ANTIALIAS  0
