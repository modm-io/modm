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

#include "arcade.hpp"

// A ship flies through an endless cave that slowly narrows. The autopilot
// looks ahead and thrusts against gravity to stay in the middle of the gap.

namespace
{

constexpr int16_t ShipX{20};

struct Particle { float x, y; int8_t life; };

struct
{
	float top[Width], bottom[Width];
	float center, drift;
	float y, vy;
	uint32_t distance;
	uint16_t crashed;
	Particle exhaust[10];
} cave_;

void
column(int16_t x)
{
	// The center wanders and the gap shrinks the further the ship flies
	cave_.drift = std::clamp(cave_.drift + (int(rnd(9)) - 4) * 0.06f, -0.9f, 0.9f);
	const float gap = std::max(11.f, 26 - cave_.distance / 300.f);
	cave_.center = std::clamp(cave_.center + cave_.drift, gap / 2 + 1, Height - gap / 2 - 1);
	if (cave_.center <= gap / 2 + 1 or cave_.center >= Height - gap / 2 - 1) cave_.drift = -cave_.drift;
	cave_.top[x] = cave_.center - gap / 2;
	cave_.bottom[x] = cave_.center + gap / 2;
}

void
reset()
{
	cave_ = {};
	cave_.center = Height / 2;
	cave_.y = Height / 2;
	for (int16_t x = 0; x < Width; x++) column(x);
}

void
draw_ship(int16_t y, bool thrust)
{
	static constexpr uint8_t Ship[4] = {0b1000000, 0b1111000, 0b1111111, 0b1111000};
	for (int16_t row = 0; row < 4; row++)
		for (int16_t col = 0; col < 7; col++)
			if (Ship[row] & (0x40 >> col)) display.setPixel(ShipX + col, y - 2 + row);
	if (thrust) display.setPixel(ShipX - 1 - rnd(2), y);
}

void
step(uint32_t frame)
{
	if (cave_.crashed)
	{
		// The ship breaks apart in an expanding ring of debris
		const int16_t r = cave_.crashed / 2;
		for (int ii = 0; ii < 16; ii++)
		{
			const float a = ii * 0.39f + cave_.crashed * 0.05f;
			display.setPixel(ShipX + 3 + std::cos(a) * r, cave_.y + std::sin(a) * r * 0.6f);
		}
		if (++cave_.crashed > 50) reset();
	}
	else
	{
		// Scroll by one pixel, two once the ship is fast
		for (int speed = cave_.distance > 2000 ? 2 : 1; speed; speed--)
		{
			std::copy(cave_.top + 1, cave_.top + Width, cave_.top);
			std::copy(cave_.bottom + 1, cave_.bottom + Width, cave_.bottom);
			column(Width - 1);
			cave_.distance++;
		}
		// Thrust if the predicted position is below the center of the gap ahead
		const int16_t ahead = ShipX + 14;
		const float target = (cave_.top[ahead] + cave_.bottom[ahead]) / 2 + (rnd(100) < 2 ? 6 : 0);
		const bool thrust = cave_.y + cave_.vy * 5 > target;
		cave_.vy = std::clamp(cave_.vy + (thrust ? -0.3f : 0.17f), -2.f, 2.f);
		cave_.y += cave_.vy;
		if (thrust) for (auto &p : cave_.exhaust) if (p.life <= 0) { p = {ShipX - 1.f, cave_.y, 8}; break; }

		for (int16_t x = ShipX; x < ShipX + 7; x++)
			if (cave_.y - 2 <= cave_.top[x] or cave_.y + 1 >= cave_.bottom[x]) cave_.crashed = 1;
		draw_ship(int16_t(cave_.y), thrust);
	}

	for (auto &p : cave_.exhaust)
	{
		if (p.life-- <= 0) continue;
		p.x -= 1.5f;
		p.y += (int(rnd(3)) - 1) * 0.5f;
		if (p.life % 2) display.setPixel(p.x, p.y);
	}

	// Rock walls with a solid edge and a dithered surface that scrolls along
	const uint32_t scroll = cave_.distance;
	for (int16_t x = 0; x < Width; x++)
	{
		const int16_t top = cave_.top[x], bottom = cave_.bottom[x];
		for (int16_t y = 0; y < Height; y++)
		{
			if (y == top or y == bottom) display.setPixel(x, y);
			else if (y < top or y > bottom) pixel(x, y, dither(x + scroll, y, 5));
		}
	}
	label(0, 0, "DIST", cave_.distance / 10);
	(void) frame;
}

}	// namespace

const Game cave{"CAVE FLYER", reset, step};
