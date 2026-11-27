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

// Breakout turned sideways for the wide screen: the paddle moves up and down
// on the left and the bricks are on the right. The autopilot follows the ball
// a little too slowly to catch every one.

namespace
{

constexpr int16_t PaddleX{3};
constexpr int16_t PaddleSize{9};
constexpr int Rows{8}, Columns{6};
constexpr int16_t BricksX{96};

struct
{
	bool bricks[Rows][Columns];
	int remaining;
	float x, y, vx, vy, speed;
	float paddle, aim;
	uint8_t lives;
	uint16_t wait;
	uint32_t score;
} game;

void
serve()
{
	game.x = PaddleX + 3;
	game.y = game.paddle;
	game.vx = game.speed;
	game.vy = (int(rnd(3)) - 1) * 0.5f;
	game.wait = 30;
}

void
fill()
{
	for (auto &row : game.bricks) for (bool &brick : row) brick = true;
	game.remaining = Rows * Columns;
}

void
reset()
{
	game = {};
	game.speed = 1.6f;
	game.paddle = Height / 2;
	game.lives = 3;
	fill();
	serve();
}

void
step(uint32_t)
{
	if (game.wait) { game.wait--; game.y = game.paddle; }
	else
	{
		game.x += game.vx;
		game.y += game.vy;
		if (game.y < 0) { game.y = -game.y; game.vy = -game.vy; }
		if (game.y > Height - 2) { game.y = 2 * (Height - 2) - game.y; game.vy = -game.vy; }
		if (game.x > Width - 2) { game.x = Width - 2; game.vx = -game.vx; }

		const int col = (int(game.x) - BricksX) / 5, row = int(game.y) / 4;
		if (game.x >= BricksX and col < Columns and row >= 0 and row < Rows and game.bricks[row][col])
		{
			game.bricks[row][col] = false;
			game.vx = -game.vx;
			game.score++;
			if (--game.remaining == 0) { game.speed *= 1.15f; fill(); serve(); }
		}

		if (game.x <= PaddleX + 2 and game.vx < 0)
		{
			const float offset = game.y + 1 - game.paddle;
			if (std::abs(offset) <= PaddleSize / 2 + 1)
			{
				// The ball leaves at an angle that depends on where it hit the paddle
				game.vx = game.speed;
				game.vy = std::clamp(offset * 0.3f, -1.6f, 1.6f);
				game.aim = int(rnd(7)) - 3;
			}
			else if (game.x < 0)
			{
				if (--game.lives == 0) reset();
				else serve();
			}
		}
	}

	// The paddle follows the ball with limited speed and a little offset
	const float target = game.vx < 0 ? game.y + game.aim : Height / 2;
	game.paddle += std::clamp(target - game.paddle, -1.1f, 1.1f);
	game.paddle = std::clamp(game.paddle, float(PaddleSize / 2), float(Height - PaddleSize / 2 - 1));

	for (int row = 0; row < Rows; row++)
		for (int col = 0; col < Columns; col++)
			if (game.bricks[row][col]) display.fillRectangle(BricksX + col * 5, row * 4, 4, 3);
	display.fillRectangle(PaddleX, int16_t(game.paddle) - PaddleSize / 2, 2, PaddleSize);
	display.fillRectangle(int16_t(game.x), int16_t(game.y), 2, 2);
	for (int ii = 0; ii < game.lives; ii++) display.fillRectangle(10 + ii * 4, 29, 2, 2);
	label(36, 0, "SCORE", game.score);
}

}	// namespace

const Game breakout{"BREAKOUT", reset, step};
