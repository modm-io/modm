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
static void _lv_modm_logging_callback(lv_log_level_t level, const char * file, uint32_t line, const char * fn_name, const char * dsc)
{
    switch (level)
    {
    case LV_LOG_LEVEL_ERROR:
        MODM_LOG_ERROR << "LVGL(" << file << ":" << line << ") " << fn_name << ": " << dsc << modm::endl;
        break;
    case LV_LOG_LEVEL_WARN:
        MODM_LOG_WARNING << "LVGL(" << file << ":" << line << ") " << fn_name << ": " << dsc << modm::endl;
        break;
    case LV_LOG_LEVEL_INFO:
        MODM_LOG_INFO << "LVGL(" << file << ":" << line << ") " << fn_name << ": " << dsc << modm::endl;
        break;
    case LV_LOG_LEVEL_TRACE:
        MODM_LOG_DEBUG << "LVGL(" << file << ":" << line << ") " << fn_name << ": " << dsc << modm::endl;
        break;
    default:
        break;
    }
}

__attribute__((constructor))
static void lv_register_modm_logging()
{
	// register modm logging callback with LVGL
	lv_log_register_print_cb(_lv_modm_logging_callback);
}
#endif
