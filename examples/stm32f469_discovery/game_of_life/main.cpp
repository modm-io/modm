/*
 * Copyright (c) 2017-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/architecture/interface/memory.hpp>

using namespace Board;
using namespace modm::glcd;
using namespace Board::ft6;

#define for_i(s, i, e) for (int i = int(s); i < int(e); i++)
#define for_xy(xx, yy) for_i(0, x, xx) for_i(0, y, yy)
#define for_yx(xx, yy) for_i(0, y, yy) for_i(0, x, xx)

// touch point coordinates
int16_t p[2][2]{{-1, -1}, {-1, -1}};
uint8_t touch_palette;
bool touch_updated = false;
// Running at full resolution is not very fast, so we need to reduce it.
#define SCALE 4
constexpr int bh = 480 / SCALE;
constexpr int bw = 800 / SCALE;
// In order to speed up simulation further, we place the _SCALEd_ framebuffers
// in Core-Coupled Memory, which is 64kB on STM32F4.
// Only SCALE 4 and higher fits the framebuffers into CCM.
constexpr modm::MemoryTraits traits = SCALE >= 4 ? modm::MemoryFastData : modm::MemoryExternal;

void read_touch()
{
	static Touch::Data touchData;
	static Touch touchSensor(touchData);
	static bool initialized = false;
	if (not initialized) {
		// Configure the touchscreen to sample with 60Hz in active and monitor mode.
		RF_CALL_BLOCKING(touchSensor.configure(Touch::InterruptMode::Polling, 60, 60));
		initialized = true;
	}

	RF_CALL_BLOCKING(touchSensor.readTouches());

	for_i(0, ii, 2)
	{
		Touch::touch_t tp;
		touchSensor.getData().getTouch(&tp, ii);

		if (tp.event == Touch::Event::Contact) {
			// rotate coordinates to ours
			p[tp.id][0] = tp.y / SCALE;
			p[tp.id][1] = (480 - tp.x) / SCALE;
			touch_updated = true;
		} else if (tp.event == Touch::Event::LiftUp) {
			// invalidate coordinates
			p[tp.id][0] = -1;
			p[tp.id][1] = -1;
			touch_palette = (touch_palette + 1) % 16;
		}
	}
}

using framebuffer_t = uint8_t (*)[bw];
uint16_t * displayBuffer;

#define TRAIL_POWER 3
#define TRAIL_LENGTH ((1 << TRAIL_POWER) + 1)
constexpr uint8_t alive = (1 << TRAIL_POWER);

#define COLOR_SHADE(red, green, blue, fraction) Color( \
	uint8_t(uint32_t(red)   * (fraction) / TRAIL_LENGTH), \
	uint8_t(uint32_t(green) * (fraction) / TRAIL_LENGTH), \
	uint8_t(uint32_t(blue)  * (fraction) / TRAIL_LENGTH) )
#define COLOR_PALETTE(r,g,b) { \
	COLOR_SHADE(r,g,b,0), COLOR_SHADE(r,g,b,1), COLOR_SHADE(r,g,b,2), COLOR_SHADE(r,g,b,3), \
	COLOR_SHADE(r,g,b,4), COLOR_SHADE(r,g,b,5), COLOR_SHADE(r,g,b,6), COLOR_SHADE(r,g,b,7), \
	COLOR_SHADE(r,g,b,8), COLOR_SHADE(r,g,b,9), COLOR_SHADE(r,g,b,10),COLOR_SHADE(r,g,b,11), \
	COLOR_SHADE(r,g,b,12),COLOR_SHADE(r,g,b,13),COLOR_SHADE(r,g,b,14),COLOR_SHADE(r,g,b,15) }

static const Color shades_of_color[][16] {
	COLOR_PALETTE(0xff, 0xff, 0xff),

	COLOR_PALETTE(0xff, 0, 0),
	COLOR_PALETTE(0, 0xff, 0),
	COLOR_PALETTE(0, 0, 0xff),

	COLOR_PALETTE(0xff, 0xff, 0),
	COLOR_PALETTE(0, 0xff, 0xff),
	COLOR_PALETTE(0xff, 0, 0xff),

	COLOR_PALETTE(0x7f, 0xff, 0),
	COLOR_PALETTE(0, 0x7f, 0xff),
	COLOR_PALETTE(0xff, 0, 0x7f),

	COLOR_PALETTE(0xff, 0x7f, 0),
	COLOR_PALETTE(0, 0xff, 0x7f),
	COLOR_PALETTE(0x7f, 0, 0xff)
};
#undef COLOR_SHADE
#undef COLOR_PALETTE

#define MAX_PALETTES 13

#define GET_TRAIL_COLOR(pixel) (shades_of_color[pixel >> 4][pixel & 0xf])
#define TRAIL_BIRTH(palette) (alive | (palette << 4))
#define IS_TRAIL_ALIVE(pixel) ((pixel) & alive)
#define TRAIL_AGE(pixel) if ((pixel & 0xf) > 1) (pixel)--;
#define GET_PALETTE(pixel) (pixel >> 4)

static inline void _touch(framebuffer_t buffer, int x, int y, uint8_t palette)
{
	static constexpr int rs = 3;

	for_i(x - rs, ix, x + rs)
		for_i(y - rs, iy, y + rs)
			if (iy >= 0 and ix >= 0 and iy < bh and ix < bw and ix % 2)
				buffer[iy][ix] = TRAIL_BIRTH(palette % MAX_PALETTES);
}


static inline void touch(framebuffer_t buffer)
{
	if (touch_updated) {
		for_i(0, ii, 2) if (p[ii][0] >= 0 and p[ii][0] >= 0) {
			_touch(buffer, p[ii][0], p[ii][1], touch_palette);
			p[ii][0] = -1;
			p[ii][1] = -1;
		}
		touch_updated = false;
	}
}

static inline void drawPixel(int x, int y, uint8_t color)
{
#define DRAW(x, y) displayBuffer[(y) * 800 + (x)] = GET_TRAIL_COLOR(color).getValue();
#if SCALE >= 8
	// >:v x:y
	// 0 |        |
	// 1 |  xxxx  |
	// 2 | x    x |
	// 3 | x xx x |
	// 4 | x xx x |
	// 5 | x    x |
	// 6 |  xxxx  |
	// 7 |        |

	// 1
	DRAW(x+2, y+1);
	DRAW(x+3, y+1);
	DRAW(x+4, y+1);
	DRAW(x+5, y+1);
	// 2
	DRAW(x+1, y+2);
	DRAW(x+6, y+2);
	// 3
	DRAW(x+1, y+3);
	DRAW(x+3, y+3);
	DRAW(x+4, y+3);
	DRAW(x+6, y+3);
	// 4
	DRAW(x+1, y+4);
	DRAW(x+3, y+4);
	DRAW(x+4, y+4);
	DRAW(x+6, y+4);
	// 5
	DRAW(x+1, y+5);
	DRAW(x+6, y+5);
	// 6
	DRAW(x+2, y+6);
	DRAW(x+3, y+6);
	DRAW(x+4, y+6);
	DRAW(x+5, y+6);
#elif SCALE >= 4
	// 0 | xx |
	// 1 |x  x|
	// 2 |x  x|
	// 3 | xx |

	// 0
	DRAW(x+1, y  );
	DRAW(x+2, y  );
	// 1
	DRAW(x+0, y+1);
	DRAW(x+3, y+1);
	// 2
	DRAW(x+0, y+2);
	DRAW(x+3, y+2);
	// 3
	DRAW(x+1, y+3);
	DRAW(x+2, y+3);
#else
	DRAW(x  , y  );
#endif
#undef DRAW
}

static inline void drawScreen(framebuffer_t before, framebuffer_t next)
{
	for_yx(bw, bh) {
		drawPixel(x*SCALE + SCALE/2, y*SCALE + SCALE/2, next[y][x]);

		before[y][x] = next[y][x];
	}
}

static inline void evolve(framebuffer_t before, framebuffer_t next)
{
	for_yx(bw, bh) {
		// counts neighbours
		int n = 0;
		// counts number of palettes in neighbours
		uint8_t pcount[16];
		memset(pcount, 0, sizeof(pcount));

		if (IS_TRAIL_ALIVE(before[y-1][x-1])) {n++; pcount[GET_PALETTE(before[y-1][x-1])]++;}
		if (IS_TRAIL_ALIVE(before[y-1][x]))   {n++; pcount[GET_PALETTE(before[y-1][x])]++;}
		if (IS_TRAIL_ALIVE(before[y-1][x+1])) {n++; pcount[GET_PALETTE(before[y-1][x+1])]++;}

		if (IS_TRAIL_ALIVE(before[y][x-1])) {n++; pcount[GET_PALETTE(before[y][x-1])]++;}
		if (IS_TRAIL_ALIVE(before[y][x+1])) {n++; pcount[GET_PALETTE(before[y][x+1])]++;}

		if (IS_TRAIL_ALIVE(before[y+1][x-1])) {n++; pcount[GET_PALETTE(before[y+1][x-1])]++;}
		if (IS_TRAIL_ALIVE(before[y+1][x]))   {n++; pcount[GET_PALETTE(before[y+1][x])]++;}
		if (IS_TRAIL_ALIVE(before[y+1][x+1])) {n++; pcount[GET_PALETTE(before[y+1][x+1])]++;}

		// age the pixel
		TRAIL_AGE(next[y][x]);

		// create pixels
		if ((n == 3) or (n == 2 and IS_TRAIL_ALIVE(before[y][x]))) {
			if (IS_TRAIL_ALIVE(before[y][x])) {pcount[GET_PALETTE(before[y][x])]++;}
			uint8_t pal = 0, max_count = 0;
			for_i(1, ii, 16) {
				if (pcount[ii] > max_count) {
					max_count = pcount[ii];
					pal = ii;
				}
			}
			next[y][x] = TRAIL_BIRTH(pal);
		}
	}
	drawScreen(before, next);
}

static inline void init(framebuffer_t *buffers)
{
	for_xy(bw, bh) {
		int ra = rand();
		if (ra < RAND_MAX / 10) {
			buffers[0][y][x] = TRAIL_BIRTH(uint8_t(ra) % MAX_PALETTES);
		} else {
			buffers[0][y][x] = 0;
		}
	}
	memset((void *) buffers[1], 0, bw*bh);
}

void game_of_life()
{
	// for raw performance we draw to our own buffer without modm::ui::GraphicDisplay!
	displayBuffer = new (modm::MemoryExternal) uint16_t[800*480];
	Board::setDisplayBuffer((void *) displayBuffer);

	// overallocating two rows and two coloums to make it possible
	// to count neighbours without bounds checking!
	framebuffer_t framebuffers[] {
		framebuffer_t(new (traits) uint8_t[(bw+2)*(bh+2)]),
		framebuffer_t(new (traits) uint8_t[(bw+2)*(bh+2)])
	};
	memset((void *) framebuffers[0], 0, (bw+2)*(bh+2));
	memset((void *) framebuffers[1], 0, (bw+2)*(bh+2));
	framebuffers[0] = framebuffer_t(uint32_t(framebuffers[0]) + bw+1);
	framebuffers[1] = framebuffer_t(uint32_t(framebuffers[1]) + bw+1);

	// 30 Hz max refresh rate, or lower
	modm::PeriodicTimer timer(33);
	bool touch_read = false;

	while (1)
	{
		// generate random input by pressing the button
		if (Button::read()) {
			init(framebuffers);
			evolve(framebuffers[0], framebuffers[1]);
			while(Button::read())
				modm::delayMilliseconds(10);
		}
		// read touch sensor during frame rate delay
		do {
			if (not touch_read) {
				read_touch();
				touch_read = true;
			}
		} while(not timer.execute());
		touch_read = false;

		// apply touch input if new
		LedBlue::set();
		touch(framebuffers[0]);
		LedBlue::reset();

		// game of life
		evolve(framebuffers[0], framebuffers[1]);
	}
}

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::initializeDisplay();
	Board::initializeTouchscreen();

	game_of_life();

	return 0;
}
