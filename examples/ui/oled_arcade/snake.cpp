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

// Snake on a 32x8 grid of 4x4 pixel cells. The autopilot finds the shortest
// path to the food, but avoids moves that would trap it in a small area.

namespace
{

constexpr int GridWidth{32}, GridHeight{8}, Cells{GridWidth * GridHeight};
constexpr int Dx[] = {1, 0, -1, 0};
constexpr int Dy[] = {0, 1, 0, -1};

struct
{
	uint8_t body[Cells];	// cell indices, head first
	int length;
	uint8_t food;
	uint16_t over;
	uint32_t eaten;
} game;

bool occupied[Cells];

int
neighbor(int cell, int direction)
{
	const int x = cell % GridWidth + Dx[direction], y = cell / GridWidth + Dy[direction];
	if (x < 0 or y < 0 or x >= GridWidth or y >= GridHeight) return -1;
	return y * GridWidth + x;
}

void
mark()
{
	std::fill(std::begin(occupied), std::end(occupied), false);
	// The tail moves away during the next step
	for (int ii = 0; ii < game.length - 1; ii++) occupied[game.body[ii]] = true;
}

int
area(int start)
{
	static bool seen[Cells];
	static uint8_t queue[Cells];
	std::copy(std::begin(occupied), std::end(occupied), std::begin(seen));
	int head = 0, tail = 0;
	queue[tail++] = start; seen[start] = true;
	while (head < tail)
	{
		const int cell = queue[head++];
		for (int d = 0; d < 4; d++)
			if (int n = neighbor(cell, d); n >= 0 and not seen[n]) { seen[n] = true; queue[tail++] = n; }
	}
	return tail;
}

// First direction of the shortest path to the food, or -1
int
path()
{
	static int16_t parent[Cells];
	static uint8_t queue[Cells];
	std::fill(std::begin(parent), std::end(parent), -1);
	int head = 0, tail = 0;
	queue[tail++] = game.body[0]; parent[game.body[0]] = game.body[0];
	while (head < tail)
	{
		const int cell = queue[head++];
		if (cell == game.food)
		{
			int current = cell;
			while (parent[current] != game.body[0]) current = parent[current];
			for (int d = 0; d < 4; d++) if (neighbor(game.body[0], d) == current) return d;
		}
		for (int d = 0; d < 4; d++)
			if (int n = neighbor(cell, d); n >= 0 and not occupied[n] and parent[n] < 0)
			{ parent[n] = cell; queue[tail++] = n; }
	}
	return -1;
}

void
place_food()
{
	do game.food = rnd(Cells);
	while ([&]{ for (int ii = 0; ii < game.length; ii++) if (game.body[ii] == game.food) return true; return false; }());
}

void
reset()
{
	game = {};
	game.length = 4;
	for (int ii = 0; ii < game.length; ii++) game.body[ii] = 3 * GridWidth + 8 - ii;
	place_food();
}

void
move()
{
	mark();
	int direction = path();
	// Only follow the path if it leaves enough room for the body afterwards
	if (direction >= 0 and area(neighbor(game.body[0], direction)) < game.length) direction = -1;
	if (direction < 0)
	{
		int best = 0;
		for (int d = 0; d < 4; d++)
			if (int n = neighbor(game.body[0], d); n >= 0 and not occupied[n])
				if (int a = area(n); a > best) { best = a; direction = d; }
	}
	if (direction < 0) { game.over = 1; return; }

	const int next = neighbor(game.body[0], direction);
	const bool eat = next == game.food;
	if (eat and game.length < Cells) game.length++;
	std::copy_backward(game.body, game.body + game.length - 1, game.body + game.length);
	game.body[0] = next;
	if (eat) { game.eaten++; place_food(); }
	// Start over before the board fills up
	if (game.length > 100) game.over = 1;
}

void
step(uint32_t frame)
{
	if (game.over)
	{
		// The snake blinks before the next round
		if (++game.over > 60) { reset(); return; }
		if (game.over / 8 % 2) return;
	}
	else if (frame % 3 == 0) move();

	for (int ii = 0; ii < game.length; ii++)
	{
		const int16_t x = game.body[ii] % GridWidth * 4, y = game.body[ii] / GridWidth * 4;
		if (ii == 0) { display.fillRectangle(x, y, 4, 4); display.clearPixel(x + 1, y + 1); }
		else display.fillRectangle(x, y, 3, 3);
	}
	const int16_t fx = game.food % GridWidth * 4, fy = game.food / GridWidth * 4;
	if (frame / 6 % 2) { display.setPixel(fx + 1, fy); display.setPixel(fx + 1, fy + 2); }
	display.setPixel(fx, fy + 1); display.setPixel(fx + 2, fy + 1);
	if (game.over) label(48, 12, "LENGTH", game.length);
}

}	// namespace

const Game snake{"SNAKE", reset, step};
