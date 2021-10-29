/*
 * Copyright (c) 2021, Thomas Sommer
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

void ColorTest::testGrayConstructors() {
	Gray8 gray8(127);
	Gray8 gray8_b(gray8);
	TEST_ASSERT_EQUALS(gray8, gray8_b);

	Gray4 gray4(7);
	gray4 += 3;
	TEST_ASSERT_EQUALS(gray4.getValue(), 10);
	gray4 -= 1;
	TEST_ASSERT_EQUALS(gray4.getValue(), 9);
	gray4 -= 42; // under-saturation
	TEST_ASSERT_EQUALS(gray4.getValue(), 0);
	gray4 += 66; // over-saturation
	TEST_ASSERT_EQUALS(gray4.getValue(), 0b00001111);
	gray4 -= 3;
	TEST_ASSERT_EQUALS(gray4.getValue(), 0b00001100);

	// not supported
	// gray4 -= -6; // another over-saturation
	// TEST_ASSERT_EQUALS(gray4.getValue(), 0b00001111);


	gray8 = gray4; // upscaling
	TEST_ASSERT_EQUALS(gray8.getValue(), 0b11001100);
	GrayD<13> gray13 = gray4; // more upscaling
	TEST_ASSERT_EQUALS(gray13.getValue(), 0b0001100110011000); // last digit rounds down for odd D
	gray4 = gray13; // downscaling
	TEST_ASSERT_EQUALS(gray4.getValue(), 0b00001100);
}

void ColorTest::testRgbConstructors() {
	Rgb888 rgb8(html::Orchid);
	Rgb888 rgb8_b(rgb8);
	TEST_ASSERT_EQUALS(rgb8, rgb8_b);

	Rgb161616 rgb16(rgb8);

	Rgb888 rgb8_c(rgb16);
	TEST_ASSERT_EQUALS(rgb8, rgb8_c);
}

void ColorTest::testHsvConstructors() {
	Hsv888 hsv8(html::Orchid);
	Hsv888 hsv8_b(hsv8);
	TEST_ASSERT_EQUALS(hsv8, hsv8_b);

	Hsv161616 hsv16(hsv8);

	Hsv888 hsv8_c(hsv16);
	TEST_ASSERT_EQUALS(hsv8, hsv8_c);
}

void ColorTest::testConvertion_8bit()
{
	Rgb888 rgb(124, 128, 10);

	Hsv888 hsv(rgb);
	TEST_ASSERT_EQUALS(hsv.getHue(), 43);
	TEST_ASSERT_EQUALS(hsv.getSaturation(), 235);
	TEST_ASSERT_EQUALS(hsv.getValue(), 128);

	Gray8 gray(rgb);
	TEST_ASSERT_EQUALS(gray.getValue(), 118);
}


void ColorTest::testConvertion_16bit()
{
	Rgb888 rgb8(html::Orchid);
	Hsv888 hsv8(rgb8);
	Hsv161616 hsv16(hsv8);

	Rgb161616 rgb16(rgb8);
	Hsv161616 hsv16_b(rgb16);

	// Test, if rgb->hsv conversion produces the same result for 8 and 16bits
	// FIXME test fails
	// TEST_ASSERT_EQUALS(hsv16, hsv16_b);
}

void ColorTest::testRgbHsvPingPongConvertion_8bit()
{
	Rgb888 rgb8(html::Orchid);
	Hsv888 hsv8(rgb8);
	Rgb888 rgb8_b(hsv8);

	// Convertion may distort - allow some tolerance.
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb8.getRed().getValue(), rgb8_b.getRed().getValue(), 1_pct));
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb8.getGreen().getValue(), rgb8_b.getGreen().getValue(), 1_pct));
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb8.getBlue().getValue(), rgb8_b.getBlue().getValue(), 1_pct));
}

void ColorTest::testRgbHsvPingPongConvertion_16bit()
{
	// Rgb->Hsv->Rgb, both 16 bit
	Rgb161616 rgb16(html::Orchid);
	Hsv161616 hsv16(rgb16);
	Rgb161616 rgb16_b(hsv16);

	// Convertion may distort - allow some tolerance.
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb16.getRed().getValue(), rgb16_b.getRed().getValue(), 1_pct));
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb16.getGreen().getValue(), rgb16_b.getGreen().getValue(), 1_pct));
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb16.getBlue().getValue(), rgb16_b.getBlue().getValue(), 1_pct));
}