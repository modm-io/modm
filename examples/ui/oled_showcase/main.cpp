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

#include <modm/board.hpp>
#include <modm/driver/display/ssd1306.hpp>
#include <modm/ui/display/font.hpp>
#include <cmath>

using namespace Board;
using namespace std::chrono_literals;

// A showcase of modm's graphics stack on a 128x32 SSD1306 OLED connected to
// I2C1 on the Arduino D14 (SDA) and D15 (SCL) pins. It cycles through a
// starfield, a big clock and an oscilloscope. Press the user button to play a
// one-button runner game, where every further press jumps.

constexpr int16_t Width{128};
constexpr int16_t Height{32};

static modm::Ssd1306<i2c::Controller, Height> display;

static void
clearRectangle(int16_t x, int16_t y, int16_t width, int16_t height)
{
	for (int16_t yy = y; yy < y + height; yy++)
		for (int16_t xx = x; xx < x + width; xx++)
			display.clearPixel(xx, yy);
}

static void
printCentered(const char *text, int16_t y)
{
	display.setCursor((Width - display.getStringWidth(text)) / 2, y);
	display << text;
}

static uint32_t
random(uint32_t range)
{
	static uint32_t state{0x9E3779B9};
	state ^= state << 13; state ^= state >> 17; state ^= state << 5;
	return state % range;
}

// ============================================================================
// Stars fly towards the viewer behind the title
struct Star { float x, y, z; };
static Star stars[48];

static void
star_reset(Star &star, float z)
{
	star.x = (random(2000) - 1000.f) / 1000.f;
	star.y = (random(2000) - 1000.f) / 1000.f;
	star.z = z;
}

static void
starfield(uint32_t frame)
{
	if (frame == 0) for (auto &star : stars) star_reset(star, random(1000) / 1000.f + 0.05f);
	for (auto &star : stars)
	{
		const int16_t x0 = Width / 2 + star.x / star.z * 64, y0 = Height / 2 + star.y / star.z * 32;
		star.z -= 0.012f;
		const int16_t x1 = Width / 2 + star.x / star.z * 64, y1 = Height / 2 + star.y / star.z * 32;
		if (star.z < 0.05f or x1 < 0 or x1 >= Width or y1 < 0 or y1 >= Height)
		{
			star_reset(star, 1.f);
			continue;
		}
		// Close stars leave a short streak
		if (star.z < 0.4f) display.drawLine(x0, y0, x1, y1);
		else display.setPixel(x1, y1);
	}
	// The title slides in from the top
	const int16_t y = std::min<int16_t>(frame - 12, 8);
	display.setFont(modm::font::ArcadeClassic);
	clearRectangle(40, y - 3, 48, 14);
	display.drawRectangle(40, y - 3, 48, 14);
	printCentered("modm", y);
}

// ============================================================================
// Uptime in large digits with blinking colons
static void
big_clock(uint32_t)
{
	const uint32_t ms = modm::Clock::now().time_since_epoch().count();
	const uint32_t seconds = ms / 1000;
	const uint8_t digits[] = {uint8_t(seconds / 36000 % 10), uint8_t(seconds / 3600 % 10),
							  uint8_t(seconds / 600 % 6), uint8_t(seconds / 60 % 10),
							  uint8_t(seconds / 10 % 6), uint8_t(seconds % 10)};
	display.setFont(modm::font::Numbers14x32);
	int16_t x = 7;
	for (int ii = 0; ii < 6; ii++)
	{
		display.setCursor(x, 0);
		display << char('0' + digits[ii]);
		x += 17;
		if (ii == 1 or ii == 3)
		{
			if (ms % 1000 < 500)
			{
				display.fillRectangle(x, 9, 3, 3);
				display.fillRectangle(x, 20, 3, 3);
			}
			x += 6;
		}
	}
}

// ============================================================================
// Two waves sweep over a dotted grid
static void
oscilloscope(uint32_t frame)
{
	for (int16_t x = 0; x < Width; x += 8)
		for (int16_t y = 0; y < Height; y += 8) display.setPixel(x, y);

	const float t = frame * 0.08f;
	int16_t previous[2]{};
	for (int16_t x = 0; x < Width; x++)
	{
		const float phase = x * 0.1f + t;
		const int16_t waves[] = {
			int16_t(15.5f + 12 * std::sin(phase) * std::cos(t * 0.3f)),
			int16_t(15.5f + 6 * std::sin(phase * 2.3f - t * 1.7f) + 5 * std::sin(phase * 0.5f)),
		};
		for (int ii = 0; ii < 2; ii++)
		{
			if (x and (ii == 0 or x % 2)) display.drawLine(x - 1, previous[ii], x, waves[ii]);
			previous[ii] = waves[ii];
		}
	}
	display.setFont(modm::font::AllCaps3x5);
	clearRectangle(0, 0, 26, 7);
	display.setCursor(1, 1);
	display << "SCOPE";
}

// ============================================================================
// One-button runner: jump over the blocks, which get faster over time
static struct
{
	float y, speed;
	float velocity;
	int16_t obstacles[3];
	uint32_t score;
	uint32_t over;
	uint8_t buffered;
} game;

static void
game_start()
{
	game = {};
	game.speed = 1.2f;
	for (int ii = 0; ii < 3; ii++) game.obstacles[ii] = Width + ii * 60 + random(30);
}

static bool
game_step(bool jump)
{
	constexpr int16_t Ground{30}, PlayerX{12};
	if (game.over)
	{
		display.setFont(modm::font::FixedWidth5x8);
		printCentered("GAME OVER", 6);
		char text[] = "SCORE 00000";
		for (uint32_t ii = 0, score = game.score / 4; ii < 5; ii++, score /= 10) text[10 - ii] = '0' + score % 10;
		printCentered(text, 18);
		return ++game.over < 60;
	}

	// A press shortly before landing still counts
	if (jump) game.buffered = 4;
	if (game.buffered and game.y == 0) { game.velocity = 3.6f; game.buffered = 0; }
	if (game.buffered) game.buffered--;
	game.y = std::max(0.f, game.y + game.velocity);
	game.velocity -= 0.3f;
	game.speed = std::min(game.speed + 0.001f, 3.f);
	game.score++;

	display.drawLine(0, Ground + 1, Width - 1, Ground + 1);
	const int16_t top = Ground - 7 - int16_t(game.y);
	display.fillRectangle(PlayerX, top, 6, 7);
	display.clearPixel(PlayerX + 4, top + 2);

	for (auto &x : game.obstacles)
	{
		x -= int16_t(game.speed + 0.5f);
		if (x < -6)
		{
			// Keep enough room to land between two blocks
			int16_t last = 0;
			for (auto other : game.obstacles) last = std::max(last, other);
			x = std::max<int16_t>(Width, last + 48 + int16_t(game.speed * 8)) + random(40);
		}
		const int16_t height = 6 + (x * 7 % 5);
		display.fillRectangle(x, Ground + 1 - height, 4, height);
		// The hitbox is one pixel smaller than the drawn block
		if (x < PlayerX + 5 and x + 4 > PlayerX + 1 and top + 6 > Ground + 1 - height) game.over = 1;
	}
	display.setFont(modm::font::AllCaps3x5);
	display.setCursor(Width - 24, 1);
	display.printf("%05lu", game.score / 4);
	return true;
}

// ============================================================================
int
main()
{
	Board::initialize();
	Board::initializeI2c();
	display.initialize();
	display.setContrast(0xFF);
	MODM_LOG_INFO << "modm OLED showcase" << modm::endl;

	using Scene = void (*)(uint32_t);
	static constexpr Scene scenes[] = {starfield, big_clock, oscilloscope};
	constexpr uint32_t SceneFrames{180};
	uint32_t scene{0}, frame{0};
	bool playing{false}, pressed{false};

	auto next_frame = modm::Clock::now();
	while (true)
	{
		const bool press = Button::read() and not pressed;
		pressed = Button::read();

		display.clear();
		if (playing) playing = game_step(press);
		else if (press) { game_start(); playing = true; }
		else
		{
			scenes[scene](frame);
			if (++frame == SceneFrames) { frame = 0; scene = (scene + 1) % std::size(scenes); }
		}
		// Sending the 512 byte frame over I2C at 400kHz takes about 12ms
		display.writeDisplay();

		next_frame += 30ms;
		modm::delay(next_frame - modm::Clock::now());
		Led::toggle();
	}
}
