/*
 * Copyright (c) 2022, Andrey Kunitsyn
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

/**
 * Pico SDK compatibility for tinyusb
 */

#include <modm/architecture/interface/assert.h>
#include <stdint.h>
/* need for static_assert at C compilation */
#include <assert.h>
#define TUD_OPT_RP2040_USB_DEVICE_ENUMERATION_FIX 0

#ifdef __cplusplus
extern "C" {
#endif
/* override expensive assert implementation */
#undef assert
#define assert(X) modm_assert((X), "pico", __FILE__ ":" MODM_STRINGIFY(__LINE__) " -> \"" #X "\"")

#ifndef hard_assert
#define hard_assert assert
#endif

void
panic(const char* /*fmt*/, ...);

#ifdef __cplusplus
}
#endif
