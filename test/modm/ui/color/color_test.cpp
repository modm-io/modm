/*
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "color_test.hpp"
#include <modm/ui/color.hpp>
#include <modm/math/tolerance.hpp>
#include <modm/debug/logger.hpp>
#include <algorithm>

using namespace modm;
using namespace modm::color;

void ColorTest::testGray() {
	Gray8 gray8(127);
	TEST_ASSERT_EQUALS(gray8.max, 255u);

	Gray8 gray8_B(gray8);
	TEST_ASSERT_EQUALS(gray8, gray8_B);

	Gray4 gray4(7);
	TEST_ASSERT_EQUALS(gray4.max, 15u);

	gray4 += 3;
	TEST_ASSERT_EQUALS(gray4, 10u);

	gray4 -= 1;
	TEST_ASSERT_EQUALS(gray4, 9u);

	gray4 -= 44; // under-saturation
	TEST_ASSERT_EQUALS(gray4, 0u);

	gray4 += 66; // over-saturation
	TEST_ASSERT_EQUALS(gray4, uint8_t(0b1111));

	gray4 -= 3;
	TEST_ASSERT_EQUALS(gray4, uint8_t(0b1100));

	// IMPLEMENT -= -
	// gray4 -= -2;

	// IMPLEMENT += -
	// gray4 += -3;

	gray8 = gray4; // upscaling
	TEST_ASSERT_EQUALS(gray8, uint8_t(0b11001100));

	Gray<13> gray13 = gray4; // further upscaling
	TEST_ASSERT_EQUALS(gray13, uint16_t(0b0001100110011000)); // last digit rounds down for odd D

	gray4 = gray13; // downscaling
	TEST_ASSERT_EQUALS(gray4, uint8_t(0b00001100));
}

void ColorTest::testRgb() {
	Rgb888 rgb888_A(0, 100, 200);
	Rgb888 rgb888_B(rgb888_A);
	TEST_ASSERT_EQUALS(rgb888_A, rgb888_B);

	Rgb161616 rgb16(rgb888_A);

	Rgb888 rgb888_C(rgb16);
	TEST_ASSERT_EQUALS(rgb888_A, rgb888_C);

	Rgb888 rgb888_d(1, 2, 3);

	rgb888_A += rgb888_d;
	TEST_ASSERT_EQUALS(rgb888_A, Rgb888(1, 102, 203));

	Rgb666 rgb666_A(1, 2, 3);
}

void ColorTest::testHsv() {
	Hsv888 hsv888(html::Orchid);
	Hsv888 hsv888_B(hsv888);
	TEST_ASSERT_EQUALS(hsv888, hsv888_B);

	Hsv161616 hsv161616(hsv888);

	Hsv888 hsv888_C(hsv161616);
	TEST_ASSERT_EQUALS(hsv888, hsv888_C);

	// Test wrapping of hue component
	Hsv<6, 6, 6> hsv666(60, 0, 0);

	hsv666.hue() += 10;
	TEST_ASSERT_EQUALS(hsv666.hue(), 6);

	hsv666.hue() -= 20;
	TEST_ASSERT_EQUALS(hsv666.hue(), 50);
}

void ColorTest::testConvertion_8bit()
{
	Rgb888 rgb(124, 128, 10);

	Hsv888 hsv(rgb);
	TEST_ASSERT_EQUALS(hsv.hue(), 43);
	TEST_ASSERT_EQUALS(hsv.saturation(), 235);
	TEST_ASSERT_EQUALS(hsv.value(), 128);

	Gray8 gray(rgb);
	TEST_ASSERT_EQUALS(gray, 118);
}

void ColorTest::testConvertion_16bit()
{
	Rgb888 rgb888(html::Orchid);
	Hsv888 hsv888(rgb888);
	Hsv161616 hsv161616(hsv888);

	Rgb161616 rgb16(rgb888);
	Hsv161616 hsv161616_B(rgb16);

	// Test, if rgb->hsv conversion produces the same result for 8 and 16bits
	// FIXME test fails
	// TEST_ASSERT_EQUALS(hsv161616, hsv161616_B);
}

void ColorTest::testRgbHsvPingPongConvertion_8bit()
{
	Rgb888 rgb888(html::Orchid);
	Hsv888 hsv888(rgb888);
	Rgb888 rgb888_B(hsv888);

	// Convertion may distort - allow some tolerance.
	TEST_ASSERT_TRUE(modm::isValueInTolerance(rgb888.red(), rgb888_B.red(), 1_pct));
	TEST_ASSERT_TRUE(modm::isValueInTolerance(rgb888.green(), rgb888_B.green(), 1_pct));
	TEST_ASSERT_TRUE(modm::isValueInTolerance(rgb888.blue(), rgb888_B.blue(), 1_pct));
}

void ColorTest::testRgbHsvPingPongConvertion_16bit()
{
	// Rgb->Hsv->Rgb, both 16 bit
	Rgb161616 rgb16(html::Orchid);
	Hsv161616 hsv16(rgb16);
	Rgb161616 rgb16_B(hsv16);

	// Convertion may distort - allow some tolerance.
	TEST_ASSERT_TRUE(modm::isValueInTolerance(rgb16.red(), rgb16_B.red(), 1_pct));
	TEST_ASSERT_TRUE(modm::isValueInTolerance(rgb16.green(), rgb16_B.green(), 1_pct));
	TEST_ASSERT_TRUE(modm::isValueInTolerance(rgb16.blue(), rgb16_B.blue(), 1_pct));
}

void ColorTest::testConstevalOperators() {

	Rgb888 rgb_a(10, 20, 30);
	Rgb888 rgb_b(40, 50, 60);

	const Rgb888 rgb_c = rgb_a + rgb_b;
	TEST_ASSERT_EQUALS(rgb_c.red(), 50);
	TEST_ASSERT_EQUALS(rgb_c.green(), 70);
	TEST_ASSERT_EQUALS(rgb_c.blue(), 90);

	const Rgb888 rgb_d = rgb_a - rgb_b;
	TEST_ASSERT_EQUALS(rgb_d.red(), 0);
	TEST_ASSERT_EQUALS(rgb_d.green(), 0);
	TEST_ASSERT_EQUALS(rgb_d.blue(), 0);
}
