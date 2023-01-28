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
#include "hardware/structs/resets.h"

#ifdef __cplusplus
extern "C" {
#endif

void
reset_block(uint32_t blocks);
void
unreset_block_wait(uint32_t blocks);

#ifdef __cplusplus
}
#endif
