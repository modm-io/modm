/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/board.hpp>
#include <modm/driver/display/ssd1306.hpp>
#include <modm/ui/display/font.hpp>
#include <algorithm>
#include <cmath>

constexpr int16_t Width{128};
constexpr int16_t Height{32};

extern modm::Ssd1306<Board::i2c::Controller, Height> display;

inline void
pixel(int16_t x, int16_t y, bool on)
{
	if (on) display.setPixel(x, y);
	else display.clearPixel(x, y);
}

/// 4x4 ordered dithering: lit if the threshold is below the brightness 0..16
inline bool
dither(int16_t x, int16_t y, float brightness)
{
	static constexpr uint8_t Bayer[4][4] = {{0, 8, 2, 10}, {12, 4, 14, 6}, {3, 11, 1, 9}, {15, 7, 13, 5}};
	return Bayer[y & 3][x & 3] < brightness;
}

/// Pseudo random number in [0, range)
uint32_t
rnd(uint32_t range);

void
clearRectangle(int16_t x, int16_t y, int16_t width, int16_t height);

void
printCentered(const char *text, int16_t y);

/// Draws a small label with a number on a black background
void
label(int16_t x, int16_t y, const char *text, uint32_t value);

/// Each game resets its state and then draws one frame per step, playing itself
struct Game
{
	const char *name;
	void (*reset)();
	void (*step)(uint32_t frame);
};

extern const Game raycaster, cave, runner, breakout, snake, stack;
