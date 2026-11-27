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

#include <modm/ui/display/monochrome_graphic_display_vertical.hpp>
#include <modm/ui/display/monochrome_graphic_display_horizontal.hpp>
#include <cstring>

#include "monochrome_display_test.hpp"

namespace
{

template< class Display >
struct Guarded
{
	struct : Display { void update() override {} using Display::getPixel; } display;
	uint8_t after[32];

	Guarded() { std::memset(after, 0xA5, sizeof(after)); }

	bool
	intact() const
	{
		for (uint8_t byte : after) if (byte != 0xA5) return false;
		return true;
	}
};

void
drawOutside(modm::GraphicDisplay &display)
{
	display.setPixel(-1, 0);
	display.setPixel(0, -1);
	display.setPixel(-200, -200);
	display.setPixel(16, 0);
	display.setPixel(0, 16);
	display.clearPixel(-1, -1);
	display.clearPixel(16, 16);
	// Lines and rectangles that start outside the display
	display.drawLine(-10, 3, 5, 3);
	display.drawLine(3, -20, 3, 5);
	display.drawLine(-10, -10, 30, 30);
	display.drawRectangle(-5, -5, 30, 30);
	// An image that extends below the display
	static const uint8_t image[] = {8, 16, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
									0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	display.drawImage({0, 8}, modm::accessor::asFlash(image));
	display.drawImage({0, -8}, modm::accessor::asFlash(image));
}

}	// namespace

void
MonochromeDisplayTest::testVerticalOutOfBounds()
{
	Guarded< modm::MonochromeGraphicDisplayVertical<16, 16> > guarded;
	drawOutside(guarded.display);
	TEST_ASSERT_TRUE(guarded.intact());
	TEST_ASSERT_FALSE(guarded.display.getPixel(-1, 0));
	TEST_ASSERT_TRUE(guarded.display.getPixel(0, 15));

	// A line starting left of the display must not wrap into the previous page
	decltype(guarded.display) &display = guarded.display;
	display.clear();
	static_cast<modm::GraphicDisplay &>(display).drawLine(-10, 10, 5, 10);
	TEST_ASSERT_TRUE(display.getPixel(0, 10));
	TEST_ASSERT_TRUE(display.getPixel(5, 10));
	TEST_ASSERT_FALSE(display.getPixel(10, 2));
	TEST_ASSERT_FALSE(display.getPixel(15, 2));
	// A line starting above the display is clipped at the top
	display.clear();
	static_cast<modm::GraphicDisplay &>(display).drawLine(3, -20, 3, 5);
	for (int16_t y = 0; y <= 5; y++) TEST_ASSERT_TRUE(display.getPixel(3, y));
	TEST_ASSERT_FALSE(display.getPixel(3, 6));
}

void
MonochromeDisplayTest::testHorizontalOutOfBounds()
{
	Guarded< modm::MonochromeGraphicDisplayHorizontal<16, 16> > guarded;
	drawOutside(guarded.display);
	TEST_ASSERT_TRUE(guarded.intact());
	TEST_ASSERT_FALSE(guarded.display.getPixel(0, -1));
	TEST_ASSERT_TRUE(guarded.display.getPixel(0, 15));
}
