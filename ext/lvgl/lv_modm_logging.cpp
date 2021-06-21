/*
 * Copyright (c) 2021 Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <lvgl/lvgl.h>
#include <modm/debug/logger.hpp>

#if LV_USE_LOG
__attribute__((constructor))
static void lv_register_modm_logging()
{
	// register modm logging callback with LVGL
	lv_log_register_print_cb([](const char *buffer) { MODM_LOG_INFO << buffer; });
}
#endif
