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

// RGB565 framebuffers in the external SDRAM, the DMA2D draw unit is enabled by default
#define LV_COLOR_DEPTH          16
#define LV_DEF_REFR_PERIOD      16
#define LV_DPI_DEF              130

#define LV_THEME_DEFAULT_DARK   1
#define LV_FONT_MONTSERRAT_12   1
#define LV_FONT_MONTSERRAT_20   1
#define LV_FONT_MONTSERRAT_28   1
