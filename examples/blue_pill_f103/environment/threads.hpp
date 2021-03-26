/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef THREADS
#define THREADS

#include "thread_blink.hpp"
#include "thread_bmp180.hpp"
#include "thread_bme280.hpp"

extern BlinkThread  blinkThread;
extern Bmp180Thread bmp180Thread;
extern Bme280Thread bme280Thread;

#endif // THREADS
