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

#pragma once

#include <modm/ui/color.hpp>
#include <modm/ui/shape/point.hpp>

using namespace modm::color;
using namespace modm::shape;

namespace modm::graphic::pattern {

	// Small collection of common patterns for inspiration
	// You can pass these to writePattern(...) methods as second argument
	constexpr int width = 16;

	constexpr auto solid = [](Point) { return html::MediumSpringGreen; };
	constexpr auto stripes_horizontal = [](Point p) { return p.x % (2 * width) < width ? html::Yellow : html::Black; };
	constexpr auto stripes_vertical = [](Point p) { return p.y % (2 * width) < width ? html::Yellow : html::Black; };
	constexpr auto stripes_diag_left = [](Point p) { return (p.x + p.y) % (2 * width) < width ? html::Yellow : html::Black; };
	constexpr auto stripes_diag_right = [](Point p) { return (p.x - p.y) % (2 * width) < width ? html::Yellow : html::Black; };

	constexpr auto gradient_hsv = [](Point p) { return Rgb888(Hsv888(p.x, p.y, 255)); };

	constexpr auto gradient_red = [](Point p) { return Rgb888(p.x + p.y / width, 30, 30); };
	constexpr auto gradient_green = [](Point p) { return Rgb888(30, p.x + p.y / width, 30); };
	constexpr auto gradient_blue = [](Point p) { return Rgb888(30, 30, p.x + p.y / width); };

	constexpr auto noize = [](Point) { return Rgb888(rand(), rand(), rand()); };
	constexpr auto white_noize = [](Point) { return GrayD<8>(rand()); };

	// Rgb888 Mandelbrot(Point p) {
	// 	return ... TODO
	// }
}