/*
 * Copyright (c) 2022, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#if __has_include("modm_config_printf.h")
#include "modm_config_printf.h"
#endif

#ifndef PRINTF_SUPPORT_LONG_LONG
#define PRINTF_SUPPORT_LONG_LONG {{ 1 if with_long_long else 0 }}
#endif

#ifndef PRINTF_DEFAULT_FLOAT_PRECISION
#define PRINTF_DEFAULT_FLOAT_PRECISION  5
#endif

#ifndef PRINTF_ALIAS_STANDARD_FUNCTION_NAMES
#define PRINTF_ALIAS_STANDARD_FUNCTION_NAMES {{ 0 if is_hosted else 1 }}
#endif

#ifndef PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_HARD
#define PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_HARD 0
#endif

#ifndef PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_SOFT
#define PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_SOFT 0
#endif


