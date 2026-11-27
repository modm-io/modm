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

// A tower grows sideways: a block slides up and down and is dropped next to
// the previous one. Only the overlap remains, the rest falls off. The
// autopilot drops with a small timing error, so the tower slowly gets thinner.

namespace
{

constexpr int16_t LayerWidth{4};
constexpr int16_t ViewLayers{24};

struct Layer { float y0, y1; };

struct
{
	Layer layers[256];
	int count;
	float y, direction, speed;
	float error;
	Layer falling;
	float fallX, fallVy;
	uint16_t over;
	uint8_t perfect;
} game;

void
next_block()
{
	const Layer &previous = game.layers[game.count - 1];
	game.y = rnd(2) ? 0 : Height - (previous.y1 - previous.y0);
	game.direction = game.y == 0 ? 1 : -1;
	// Perfect drops get rarer the faster the block moves
	game.error = rnd(3) ? (int(rnd(5)) - 2) * game.speed * 0.6f : 0;
}

void
reset()
{
	game = {};
	game.layers[0] = {8, 24};
	game.count = 1;
	game.speed = 0.6f;
	next_block();
}

int16_t
column(int layer)
{
	const int first = std::max(0, game.count - ViewLayers);
	return 2 + (layer - first) * LayerWidth;
}

void
step(uint32_t)
{
	const Layer &previous = game.layers[game.count - 1];
	const float length = previous.y1 - previous.y0;

	if (game.over)
	{
		if (++game.over > 70) reset();
	}
	else
	{
		game.y += game.direction * game.speed;
		if (game.y <= 0 or game.y + length >= Height) game.direction = -game.direction;
		// Drop when the block passes the previous layer plus the timing error
		const float target = std::clamp(previous.y0 + game.error, 0.f, Height - length);
		if (std::abs(game.y - target) < game.speed / 2 + 0.01f)
		{
			const Layer block{game.y, game.y + length};
			const Layer overlap{std::max(block.y0, previous.y0), std::min(block.y1, previous.y1)};
			if (overlap.y1 - overlap.y0 < 1) game.over = 1;
			else
			{
				// The cut off part falls away from the tower
				game.falling = block.y0 < previous.y0 ? Layer{block.y0, overlap.y0} : Layer{overlap.y1, block.y1};
				game.fallX = column(game.count);
				game.fallVy = block.y0 < previous.y0 ? -0.5f : 0.5f;
				const bool perfect = std::abs(target - previous.y0) < 0.5f;
				game.perfect = perfect ? 10 : 0;
				if (perfect) game.falling = {};
				game.layers[game.count++] = perfect ? previous : overlap;
				game.speed = std::min(game.speed + 0.03f, 2.2f);
				if (game.count == int(std::size(game.layers))) game.over = 1;
				next_block();
			}
		}
	}

	const int first = std::max(0, game.count - ViewLayers);
	for (int ii = first; ii < game.count; ii++)
	{
		const Layer &layer = game.layers[ii];
		display.fillRectangle(column(ii), layer.y0, LayerWidth - 1, layer.y1 - layer.y0);
	}
	if (not game.over)
	{
		const int16_t x = column(game.count);
		for (int16_t y = game.y; y < game.y + length; y++)
			for (int16_t dx = 0; dx < LayerWidth - 1; dx++) pixel(x + dx, y, true);
	}
	if (game.falling.y1 > game.falling.y0)
	{
		game.falling.y0 += game.fallVy; game.falling.y1 += game.fallVy;
		game.fallVy *= 1.15f;
		game.fallX += 0.4f;
		// Forget the piece once it left the screen, before its position overflows
		if (game.falling.y1 < 0 or game.falling.y0 > Height) game.falling = {};
		const int16_t y0 = std::max(0.f, game.falling.y0), y1 = std::min<float>(Height, game.falling.y1);
		for (int16_t y = y0; y < y1; y++)
			for (int16_t dx = 0; dx < LayerWidth - 1; dx++) pixel(game.fallX + dx, y, dither(game.fallX + dx, y, 8));
	}
	// A perfect drop flashes a frame around the new layer
	if (game.perfect and game.perfect--)
	{
		const Layer &layer = game.layers[game.count - 1];
		display.drawRectangle(column(game.count - 1) - 1, layer.y0 - 1, LayerWidth + 1, layer.y1 - layer.y0 + 2);
	}
	label(Width - 44, 0, "HEIGHT", game.count - 1);
}

}	// namespace

const Game stack{"STACK", reset, step};
