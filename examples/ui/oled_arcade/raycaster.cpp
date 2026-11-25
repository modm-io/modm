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

// A maze crawler: the camera follows the wall on its right, while hovering
// creatures drift through the corridors. Whenever one is in the crosshair,
// the autopilot fires at it. The walls are raycast with procedural textures
// and shaded with ordered dithering.

namespace
{

constexpr int16_t Horizon{Height / 2};
// Projection scale of walls and sprites, taller than the view for the wide 4:1 screen
constexpr float Scale{Height * 1.5f};

// ============================================================================
constexpr const char *Map[] = {
	"################",
	"#......#.......#",
	"#.####.#.#####.#",
	"#.#..........#.#",
	"#.#.%%%.==.#.#.#",
	"#...%......#...#",
	"###.%.====.###.#",
	"#.....=..=.....#",
	"#.##..=..=..##.#",
	"#.#.......#..#.#",
	"#...###.#....#.#",
	"################",
};
constexpr int MapWidth{16};
constexpr int MapHeight{std::size(Map)};

static char
cell(int x, int y)
{
	if (x < 0 or y < 0 or x >= MapWidth or y >= MapHeight) return '#';
	return Map[y][x];
}

static bool
open(int x, int y) { return cell(x, y) == '.'; }

// Procedural 16x16 wall textures, true is a lit texel. Coarse patterns stay
// readable on a wall that is only a few pixels tall.
static bool
texture(char type, int u, int v)
{
	switch (type)
	{
		case '%': // large stone blocks
			return v % 8 != 0 and (u + (v / 8) * 6) % 12 != 0;
		case '=': // metal panels with a seam in the middle
			return u % 8 != 0 and v != 8;
		default: // bricks
			return v % 4 != 0 and (u + (v / 4 % 2) * 4) % 8 != 0;
	}
}

// ============================================================================
// The camera walks from cell center to cell center and keeps a wall on its right
static struct
{
	float x{1.5f}, y{1.5f}, angle{0};
	int cx{1}, cy{1}, heading{0};
	float bob;
} camera;

constexpr int Dx[] = {1, 0, -1, 0};
constexpr int Dy[] = {0, 1, 0, -1};

static float
wrap(float angle)
{
	while (angle > float(M_PI)) angle -= 2 * float(M_PI);
	while (angle < -float(M_PI)) angle += 2 * float(M_PI);
	return angle;
}

static void
camera_step(bool aiming)
{
	const float tx = camera.cx + 0.5f, ty = camera.cy + 0.5f;
	if (std::hypot(tx - camera.x, ty - camera.y) < 0.02f)
	{
		camera.x = tx; camera.y = ty;
		for (int turn : {1, 0, 3, 2})
		{
			const int heading = (camera.heading + turn) % 4;
			if (open(camera.cx + Dx[heading], camera.cy + Dy[heading]))
			{
				camera.heading = heading;
				camera.cx += Dx[heading];
				camera.cy += Dy[heading];
				break;
			}
		}
	}
	if (aiming) return;
	// Turn smoothly towards the next cell and only walk when facing it
	const float target = std::atan2(camera.cy + 0.5f - camera.y, camera.cx + 0.5f - camera.x);
	const float delta = wrap(target - camera.angle);
	camera.angle = wrap(camera.angle + std::clamp(delta, -0.09f, 0.09f));
	if (std::abs(delta) < 0.3f)
	{
		const float distance = std::hypot(camera.cx + 0.5f - camera.x, camera.cy + 0.5f - camera.y);
		const float step = std::min(0.045f, distance);
		camera.x += std::cos(target) * step;
		camera.y += std::sin(target) * step;
		camera.bob += 0.35f;
	}
}

// ============================================================================
// Hovering one-eyed creatures, drawn as 16x16 procedural sprites
struct Creature
{
	float x, y, phase;
	uint8_t hit;		// frames since it was hit, 0 if alive
	uint16_t respawn;	// frames until it appears again
};
static Creature creatures[4];
static uint32_t kills;

static void
creature_place(Creature &creature)
{
	int x, y;
	do { x = rnd(MapWidth); y = rnd(MapHeight); }
	while (not open(x, y) or std::hypot(x + 0.5f - camera.x, y + 0.5f - camera.y) < 4);
	creature = {x + 0.5f, y + 0.5f, float(rnd(628)) / 100, 0, 0};
}

// Returns 0 for transparent, 1 for black, 2 for lit, 3 for half lit
static uint8_t
creature_texel(int u, int v, float look)
{
	const float dx = u - 7.5f, dy = v - 7.f, d = std::sqrt(dx * dx + dy * dy);
	if (d > 7.5f)
	{
		// Three little tentacles below the body
		if (v > 12 and (u == 4 or u == 8 or u == 11) and ((u + v) % 3) != 0) return 2;
		return 0;
	}
	if (d > 6.5f) return 2;
	const float px = u - (7.5f + look * 2), py = v - 6.5f, pupil = std::sqrt(px * px + py * py);
	if (pupil < 1.6f) return 1;
	if (pupil < 3.2f) return 2;
	return 3;
}

// ============================================================================
static float zbuffer[Width];

static void
render_walls()
{
	const float dirX = std::cos(camera.angle), dirY = std::sin(camera.angle);
	const float planeX = -dirY * 0.66f, planeY = dirX * 0.66f;
	const int16_t bob = int16_t(std::sin(camera.bob) * 0.8f);

	for (int16_t x = 0; x < Width; x++)
	{
		const float cameraX = 2 * x / float(Width) - 1;
		const float rayX = dirX + planeX * cameraX, rayY = dirY + planeY * cameraX;
		int mapX = int(camera.x), mapY = int(camera.y);
		const float deltaX = std::abs(1 / rayX), deltaY = std::abs(1 / rayY);
		const int stepX = rayX < 0 ? -1 : 1, stepY = rayY < 0 ? -1 : 1;
		float sideX = (rayX < 0 ? camera.x - mapX : mapX + 1 - camera.x) * deltaX;
		float sideY = (rayY < 0 ? camera.y - mapY : mapY + 1 - camera.y) * deltaY;
		bool side;
		do {
			if (sideX < sideY) { sideX += deltaX; mapX += stepX; side = false; }
			else { sideY += deltaY; mapY += stepY; side = true; }
		} while (open(mapX, mapY));

		const float distance = side ? sideY - deltaY : sideX - deltaX;
		zbuffer[x] = distance;
		const int16_t lineHeight = Scale / distance;
		const int16_t top = Horizon - lineHeight / 2 + bob;
		float wallX = side ? camera.x + distance * rayX : camera.y + distance * rayY;
		wallX -= std::floor(wallX);
		const int u = int(wallX * 16);
		// Darker with distance and on one side for depth
		// Solid up close, dithered into the distance, side walls at half brightness
		const float brightness = std::clamp(24 - distance * 4.5f, 1.5f, 16.f) * (side ? 0.5f : 1.f);

		for (int16_t y = 0; y < Height; y++)
		{
			if (y < top) display.clearPixel(x, y);
			else if (y < top + lineHeight)
			{
				const int v = (y - top) * 16 / lineHeight;
				pixel(x, y, texture(cell(mapX, mapY), u, v) and dither(x, y, brightness));
			}
			else display.clearPixel(x, y);
		}
	}
}

// Returns the creature in the crosshair, if any
static Creature *
render_creatures(uint32_t frame)
{
	const float dirX = std::cos(camera.angle), dirY = std::sin(camera.angle);
	const float planeX = -dirY * 0.66f, planeY = dirX * 0.66f;
	const float inverse = 1 / (planeX * dirY - dirX * planeY);

	// Draw the far ones first
	Creature *order[std::size(creatures)];
	for (size_t ii = 0; ii < std::size(creatures); ii++) order[ii] = &creatures[ii];
	const auto distance = [](const Creature *c) { return std::hypot(c->x - camera.x, c->y - camera.y); };
	std::sort(std::begin(order), std::end(order), [&](auto *a, auto *b) { return distance(a) > distance(b); });

	Creature *target{nullptr};
	for (Creature *c : order)
	{
		if (c->respawn) continue;
		const float rx = c->x - camera.x, ry = c->y - camera.y;
		const float tx = inverse * (dirY * rx - dirX * ry);
		const float depth = inverse * (-planeY * rx + planeX * ry);
		if (depth < 0.3f) continue;

		const int16_t screenX = int16_t((Width / 2) * (1 + tx / depth));
		// Shrink and blink after a hit
		float scale = 0.6f;
		if (c->hit) scale *= 1 + c->hit * 0.12f;
		const int16_t size = int16_t(Scale / depth * scale);
		const int16_t hover = int16_t(std::sin(frame * 0.08f + c->phase) * 4 / depth);
		const int16_t top = Horizon - size / 2 + int16_t(Scale / depth * 0.08f) + hover;
		const float look = std::clamp(-tx / depth, -1.f, 1.f);

		for (int16_t x = std::max<int16_t>(0, screenX - size / 2); x < std::min<int16_t>(Width, screenX + size / 2); x++)
		{
			if (depth >= zbuffer[x]) continue;
			const int u = (x - (screenX - size / 2)) * 16 / size;
			for (int16_t y = std::max<int16_t>(0, top); y < std::min<int16_t>(Height, top + size); y++)
			{
				const int v = (y - top) * 16 / size;
				uint8_t texel = creature_texel(u, v, look);
				// An exploding creature turns into a sparse cloud of pixels
				if (c->hit and texel and rnd(10) < c->hit) texel = 0;
				if (texel == 1) display.clearPixel(x, y);
				else if (texel == 2) display.setPixel(x, y);
				else if (texel == 3) pixel(x, y, dither(x, y, 6));
			}
		}
		if (not c->hit and std::abs(screenX - Width / 2) < size / 3 and depth < 3.5f) target = c;
	}
	return target;
}

static void
render_hud(uint8_t firing)
{
	// Crosshair
	for (int16_t d : {2, 3}) { pixel(64 + d, Horizon, true); pixel(64 - d, Horizon, true); pixel(64, Horizon + d, true); pixel(64, Horizon - d, true); }

	// A small blaster that sways while walking and recoils when firing
	const int16_t sway = int16_t(std::sin(camera.bob * 0.5f) * 2);
	const int16_t recoil = firing > 4 ? 2 : 0;
	const int16_t gx = 74 + sway, gy = 24 + recoil + int16_t(std::abs(std::cos(camera.bob * 0.5f)));
	for (int16_t y = gy - 1; y < Height; y++) for (int16_t x = gx - 1; x < gx + 10; x++) display.clearPixel(x, y);
	display.fillRectangle(gx + 3, gy, 3, 5);
	display.fillRectangle(gx, gy + 4, 9, Height - gy - 4);
	display.clearPixel(gx + 4, gy + 2);
	if (firing > 4)
	{
		display.fillCircle({int16_t(gx + 4), int16_t(gy - 3)}, 2);
		for (int16_t r : {4, 5})
		{
			pixel(gx + 4 - r, gy - 3, true); pixel(gx + 4 + r, gy - 3, true);
			pixel(gx + 4, gy - 3 - r, true);
		}
	}

	label(0, 0, "KILLS", kills);
}

static uint8_t firing;

void
reset()
{
	camera = {};
	kills = 0;
	firing = 0;
	for (auto &creature : creatures) creature_place(creature);
}

void
step(uint32_t frame)
{
	render_walls();
	Creature *target = render_creatures(frame);

	// Stop, aim and fire at creatures in the crosshair
	if (target and not firing)
	{
		firing = 8;
		if (rnd(2)) target->hit = 1;
	}
	camera_step(target != nullptr);
	if (firing) firing--;

	for (auto &creature : creatures)
	{
		if (creature.respawn and not --creature.respawn) creature_place(creature);
		else if (creature.hit and ++creature.hit > 10) { creature.respawn = 450; creature.hit = 0; kills++; }
	}
	render_hud(firing);
}

}	// namespace

const Game raycaster{"CRAWLER", reset, step};
