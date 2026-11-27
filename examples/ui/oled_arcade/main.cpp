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

using namespace Board;
using namespace std::chrono_literals;

// An arcade of self-playing games on a 128x32 SSD1306 OLED connected to I2C1
// on the Arduino D14 (SDA) and D15 (SCL) pins, drawn with modm's graphics stack.
// Each game plays for two minutes, then the next one starts. The user button
// skips to the next game.

modm::Ssd1306<i2c::Controller, Height> display;

uint32_t
rnd(uint32_t range)
{
	static uint32_t state{0x2545F491};
	state ^= state << 13; state ^= state >> 17; state ^= state << 5;
	return state % range;
}

void
clearRectangle(int16_t x, int16_t y, int16_t width, int16_t height)
{
	for (int16_t yy = y; yy < y + height; yy++)
		for (int16_t xx = x; xx < x + width; xx++) display.clearPixel(xx, yy);
}

void
printCentered(const char *text, int16_t y)
{
	display.setCursor((Width - display.getStringWidth(text)) / 2, y);
	display << text;
}

void
label(int16_t x, int16_t y, const char *text, uint32_t value)
{
	char digits[12];
	int length = 0;
	do { digits[length++] = '0' + value % 10; value /= 10; } while (value);
	display.setFont(modm::font::AllCaps3x5);
	clearRectangle(x, y, display.getStringWidth(text) + 4 * length + 4, 7);
	display.setCursor(x + 1, y + 1);
	display << text << ' ';
	while (length) display << digits[--length];
}

// ============================================================================
constexpr const Game *Games[] = {&raycaster, &cave, &runner, &breakout, &snake, &stack};
constexpr uint32_t FramesPerGame{2 * 60 * 30};
constexpr uint32_t TitleFrames{60};

static void
title(const Game &game, uint32_t index, uint32_t frame)
{
	display.setFont(modm::font::ArcadeClassic);
	printCentered(game.name, 7);
	char text[] = "AUTO PLAY 0/0";
	text[10] = '1' + index;
	text[12] = '0' + std::size(Games);
	display.setFont(modm::font::AllCaps3x5);
	printCentered(text, 22);
	// A bar fills up until the game starts
	display.fillRectangle(0, 30, frame * Width / TitleFrames, 2);
}

int
main()
{
	Board::initialize();
	Board::initializeI2c();
	display.initialize();
	display.setContrast(0xFF);
	MODM_LOG_INFO << "modm OLED arcade" << modm::endl;

	uint32_t index{0}, frame{0};
	bool pressed{false};
	auto next_frame = modm::Clock::now();
	while (true)
	{
		const Game &game = *Games[index];
		const bool press = Button::read() and not pressed;
		pressed = Button::read();

		display.clear();
		if (frame < TitleFrames) title(game, index, frame);
		else
		{
			if (frame == TitleFrames) game.reset();
			game.step(frame - TitleFrames);
		}
		// Sending the 512 byte frame over I2C at 400kHz takes about 12ms
		display.writeDisplay();

		if (++frame >= TitleFrames + FramesPerGame or press)
		{
			frame = 0;
			index = (index + 1) % std::size(Games);
		}

		next_frame += 33ms;
		const auto now = modm::Clock::now();
		if (next_frame > now) modm::delay(next_frame - now);
		else next_frame = now;
	}
}
