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

// A block runs over the ground and jumps over obstacles that get faster.
// The autopilot jumps just in time, but sometimes hesitates.

namespace
{

constexpr int16_t Ground{30};
constexpr int16_t PlayerX{14};

struct
{
	float y, velocity, speed;
	int16_t obstacles[3];
	bool hesitate[3];
	uint32_t score;
	uint16_t over;
} game;

void
place(int ii, int16_t after)
{
	game.obstacles[ii] = std::max<int16_t>(Width, after + 48 + int16_t(game.speed * 8)) + rnd(40);
	game.hesitate[ii] = rnd(100) < 4;
}

void
reset()
{
	game = {};
	game.speed = 1.2f;
	for (int ii = 0; ii < 3; ii++) place(ii, ii ? game.obstacles[ii - 1] : 0);
}

void
step(uint32_t frame)
{
	if (game.over)
	{
		display.setFont(modm::font::FixedWidth5x8);
		printCentered("GAME OVER", 6);
		label(44, 19, "SCORE", game.score / 4);
		if (++game.over > 60) reset();
		return;
	}

	// Jump when the next obstacle is close enough for the current speed
	for (int ii = 0; ii < 3; ii++)
	{
		const int16_t distance = game.obstacles[ii] - (PlayerX + 6);
		if (distance > 0 and distance < game.speed * 7 + 2 and game.y == 0 and not game.hesitate[ii])
			game.velocity = 3.6f;
	}
	game.y = std::max(0.f, game.y + game.velocity);
	game.velocity -= 0.3f;
	if (game.y == 0) game.velocity = 0;
	game.speed = std::min(game.speed + 0.001f, 3.f);
	game.score++;

	// Dashed ground that moves with the speed
	for (int16_t x = 0; x < Width; x++)
		if ((x + int16_t(game.score * game.speed)) % 6) display.setPixel(x, Ground + 1);
	const int16_t top = Ground - 7 - int16_t(game.y);
	display.fillRectangle(PlayerX, top, 6, 7);
	display.clearPixel(PlayerX + 4, top + 2);
	// Legs alternate while running
	if (game.y == 0) display.clearPixel(PlayerX + 1 + (frame / 4 % 2) * 3, Ground);

	for (int ii = 0; ii < 3; ii++)
	{
		int16_t &x = game.obstacles[ii];
		x -= int16_t(game.speed + 0.5f);
		if (x < -6)
		{
			int16_t last = 0;
			for (int16_t other : game.obstacles) last = std::max(last, other);
			place(ii, last);
		}
		const int16_t height = 6 + (ii * 3 + x / 64) % 4;
		display.fillRectangle(x, Ground + 1 - height, 4, height);
		if (x < PlayerX + 5 and x + 4 > PlayerX + 1 and top + 6 > Ground + 1 - height) game.over = 1;
	}
	label(Width - 40, 0, "SCORE", game.score / 4);
}

}	// namespace

const Game runner{"RUNNER", reset, step};
