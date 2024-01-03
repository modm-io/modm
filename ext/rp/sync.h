/*
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

// Stripped down header "hardware/sync.h" for tinyusb

#define remove_volatile_cast(t, x) ({ __asm volatile ("dmb" ::: "memory"); (t)(x); })
