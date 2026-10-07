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

#ifndef TEST_SCREEN_H
#define TEST_SCREEN_H

#pragma once

#include <cstdint>

void drawTestScreen(void);
void setLblText(void);
void setTouchText(int16_t x, int16_t y, int16_t rawX, int16_t rawY);


#endif //TEST_SCREEN_H
