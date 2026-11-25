/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef LV_CONF_H
#	error	"Don't include this file directly, use 'lv_conf.h' instead!"
#endif

// The display uses RGB565 framebuffers in the external SDRAM
#define LV_COLOR_DEPTH          16
// Refresh as fast as the 60 Hz panel can show it
#define LV_DEF_REFR_PERIOD      16
#define LV_DPI_DEF              160

#define LV_USE_LOG              1
#define LV_LOG_LEVEL            LV_LOG_LEVEL_WARN

#define LV_THEME_DEFAULT_DARK   1
#define LV_FONT_MONTSERRAT_20   1
#define LV_FONT_MONTSERRAT_28   1
#define LV_FONT_MONTSERRAT_48   1

// Render the static gauge dial once into an image
#define LV_USE_SNAPSHOT         1

// The DMA2D draw unit is enabled by default on devices that have one.
// Uncomment to compare the render time with software rendering only.
// #define LV_USE_DRAW_DMA2D    0
