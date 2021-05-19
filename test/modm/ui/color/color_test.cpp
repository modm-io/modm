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

using namespace modm::color;

void ColorTest::testRgbCopyConstructors() {
	RgbT<uint8_t> rgb8(html::Orchid);
	RgbT<uint8_t> rgb8_b(rgb8);
	TEST_ASSERT_EQUALS(rgb8, rgb8_b);

	RgbT<uint16_t> rgb16(rgb8);
	TEST_ASSERT_EQUALS(uint16_t(rgb8.red) << 8, rgb16.red);
	TEST_ASSERT_EQUALS(uint16_t(rgb8.green) << 8, rgb16.green);
	TEST_ASSERT_EQUALS(uint16_t(rgb8.blue) << 8, rgb16.blue);

	RgbT<uint8_t> rgb8_c(rgb16);
	TEST_ASSERT_EQUALS(rgb8, rgb8_c);
}

void ColorTest::testHsvCopyConstructors() {
	HsvT<uint8_t> hsv8(html::Orchid);
	HsvT<uint8_t> hsv8_b(hsv8);
	TEST_ASSERT_EQUALS(hsv8, hsv8_b);

	HsvT<uint16_t> hsv16(hsv8);
	TEST_ASSERT_EQUALS(uint16_t(hsv8.hue) << 8, hsv16.hue);
	TEST_ASSERT_EQUALS(uint16_t(hsv8.saturation) << 8, hsv16.saturation);
	TEST_ASSERT_EQUALS(uint16_t(hsv8.value) << 8, hsv16.value);

	HsvT<uint8_t> hsv8_c(hsv16);
	TEST_ASSERT_EQUALS(hsv8, hsv8_c);
}

void ColorTest::testBrightnessCopyConstructors() {
	BrightnessT<uint8_t> brightness8(127);
	BrightnessT<uint8_t> brightness8_b(brightness8);
	TEST_ASSERT_EQUALS(brightness8.value, brightness8_b.value);

	BrightnessT<uint16_t> brightness16(brightness8);
	TEST_ASSERT_EQUALS(uint16_t(brightness8.value) << 8, brightness16.value);

	BrightnessT<uint8_t> brightness8_c(brightness16);
	TEST_ASSERT_EQUALS(brightness8.value, brightness8_c.value);
}

void ColorTest::testConvertion_8bit()
{
	RgbT<uint8_t> rgb(124, 128, 10);

	HsvT<uint8_t> hsv(rgb);
	TEST_ASSERT_EQUALS(hsv.hue, 43);
	TEST_ASSERT_EQUALS(hsv.saturation, 235);
	TEST_ASSERT_EQUALS(hsv.value, 128);

	BrightnessT<uint8_t> brightness(rgb);
	TEST_ASSERT_EQUALS(brightness.value, 118);
}

// TODO 16bit convertion not yet working
// see hsv_impl.hpp and rgb_impl.hpp
// void ColorTest::testConvertion_16bit()
// {
// 	RgbT<uint8_t> rgb8(html::Orchid);
// 	HsvT<uint8_t> hsv8(rgb8);
// 	HsvT<uint16_t> hsv16(hsv8);

// 	RgbT<uint16_t> rgb16(rgb8);
// 	HsvT<uint16_t> hsv16_b(rgb16);

// 	// Test, if rgb->hsv conversion produces the same result for 8 and 16bits
// 	TEST_ASSERT_EQUALS(hsv16, hsv16_b);
// }

void ColorTest::testRgbHsvPingPongConvertion_8bit()
{
	RgbT<uint8_t> rgb8(html::Orchid);
	HsvT<uint8_t> hsv8(rgb8);
	RgbT<uint8_t> rgb8_b(hsv8);

	// Convertion can distort - allow some tolerance.
	using namespace modm;
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb8.red, rgb8_b.red, 1_pct));
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb8.green, rgb8_b.green, 1_pct));
	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb8.blue, rgb8_b.blue, 1_pct));
}

// TODO 16bit convertion not yet working
// see hsv_impl.hpp and rgb_impl.hpp
// void ColorTest::testRgbHsvPingPongConvertion_16bit()
// {
// 	// Rgb->Hsv->Rgb, both 16 bit
// 	RgbT<uint16_t> rgb16(html::Orchid);
// 	HsvT<uint16_t> hsv16(rgb16);
// 	RgbT<T> rgb16_b(hsv16);

// 	// Convertion can distort - allow some tolerance.
// 	using namespace modm;
// 	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb.red, rgb16_b.red, 1_pct));
// 	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb.green, rgb16_b.green, 1_pct));
// 	TEST_ASSERT_TRUE(modm::Tolerance::isValueInTolerance(rgb.blue, rgb16_b.blue, 1_pct));
// }