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

#include "rgb.hpp"

/// @ingroup     modm_ui_color
namespace modm::color::html
{

/**
 * Constant HTML Colornames in RGB Colorspace
 *
 * @see 		https://htmlcolorcodes.com/color-names/
 * @see 		modm:color:RgbT
 * @ingroup 	modm_ui_color
 * @{
 */

// Red HTML Color Names
static constexpr Rgb888 IndianRed(205, 92, 92);
static constexpr Rgb888 LightCoral(240, 128, 128);
static constexpr Rgb888 Salmon(250, 128, 114);
static constexpr Rgb888 DarkSalmon(233, 150, 122);
static constexpr Rgb888 LightSalmon(255, 160, 122);
static constexpr Rgb888 Crimson(220, 20, 60);
static constexpr Rgb888 Red(255, 0, 0);
static constexpr Rgb888 FireBrick(178, 34, 34);
static constexpr Rgb888 DarkRed(139, 0, 0);

// Pink HTML Color Names
static constexpr Rgb888 Pink(255, 192, 203);
static constexpr Rgb888 LightPink(255, 182, 193);
static constexpr Rgb888 HotPink(255, 105, 180);
static constexpr Rgb888 DeepPink(255, 20, 147);
static constexpr Rgb888 MediumVioletRed(199, 21, 133);
static constexpr Rgb888 PaleVioletRed(219, 112, 147);

// Orange HTML Color Names
static constexpr Rgb888 Coral(255, 127, 80);
static constexpr Rgb888 Tomato(255, 99, 71);
static constexpr Rgb888 OrangeRed(255, 69, 0);
static constexpr Rgb888 DarkOrange(255, 140, 0);
static constexpr Rgb888 Orange(25, 165, 0);

// Yellow HTML Color Names
static constexpr Rgb888 Gold(255, 215, 0);
static constexpr Rgb888 Yellow(255, 255, 0);
static constexpr Rgb888 LightYellow(255, 255, 224);
static constexpr Rgb888 LemonChiffon(255, 250, 205);
static constexpr Rgb888 LightGoldenrodYellow(250, 250, 210);
static constexpr Rgb888 PapayaWhip(255, 239, 213);
static constexpr Rgb888 Moccasin(255, 228, 181);
static constexpr Rgb888 PeachPuff(255, 218, 185);
static constexpr Rgb888 PaleGoldenrod(238, 232, 170);
static constexpr Rgb888 Khaki(240, 230, 140);
static constexpr Rgb888 DarkKhaki(189, 183, 107);

// Purple HTML Color Names
static constexpr Rgb888 Lavender(230, 230, 250);
static constexpr Rgb888 Thistle(216, 191, 216);
static constexpr Rgb888 Plum(221, 160, 221);
static constexpr Rgb888 Violet(238, 130, 238);
static constexpr Rgb888 Orchid(218, 112, 214);
static constexpr Rgb888 Fuchsia(255, 0, 255);
static constexpr Rgb888 Magenta(255, 0, 255);
static constexpr Rgb888 MediumOrchid(186, 85, 211);
static constexpr Rgb888 MediumPurple(147, 112, 219);
static constexpr Rgb888 RebeccaPurple(102, 51, 153);
static constexpr Rgb888 BlueViolet(138, 43, 226);
static constexpr Rgb888 DarkViolet(148, 0, 211);
static constexpr Rgb888 DarkOrchid(153, 50, 204);
static constexpr Rgb888 DarkMagenta(139, 0, 139);
static constexpr Rgb888 Purple(128, 0, 128);
static constexpr Rgb888 Indigo(75, 0, 130);
static constexpr Rgb888 SlateBlue(106, 90, 205);
static constexpr Rgb888 DarkSlateBlue(72, 61, 139);

// Green HTML Color Names
static constexpr Rgb888 GreenYellow(173, 255, 47);
static constexpr Rgb888 Chartreuse(127, 255, 0);
static constexpr Rgb888 LawnGreen(124, 252, 0);
static constexpr Rgb888 Lime(0, 255, 0);
static constexpr Rgb888 LimeGreen(50, 205, 50);
static constexpr Rgb888 PaleGreen(152, 251, 152);
static constexpr Rgb888 LightGreen(144, 238, 144);
static constexpr Rgb888 MediumSpringGreen(0, 250, 154);
static constexpr Rgb888 SpringGreen(0, 255, 127);
static constexpr Rgb888 MediumSeaGreen(60, 179, 113);
static constexpr Rgb888 SeaGreen(46, 139, 87);
static constexpr Rgb888 ForestGreen(34, 139, 34);
static constexpr Rgb888 Green(0, 128, 0);
static constexpr Rgb888 DarkGreen(0, 100, 0);
static constexpr Rgb888 YellowGreen(154, 205, 50);
static constexpr Rgb888 OliveDrab(107, 142, 35);
static constexpr Rgb888 Olive(128, 128, 0);
static constexpr Rgb888 DarkOliveGreen(85, 107, 47);
static constexpr Rgb888 MediumAquamarine(102, 205, 170);
static constexpr Rgb888 DarkSeaGreen(143, 188, 139);
static constexpr Rgb888 LightSeaGreen(32, 178, 170);
static constexpr Rgb888 DarkCyan(0, 139, 139);
static constexpr Rgb888 Teal(0, 128, 128);

// Blue HTML Color Names
static constexpr Rgb888 Aqua(0, 255, 255);
static constexpr Rgb888 Cyan(0, 255, 255);
static constexpr Rgb888 LightCyan(224, 255, 255);
static constexpr Rgb888 PaleTurquoise(175, 238, 238);
static constexpr Rgb888 Aquamarine(127, 255, 212);
static constexpr Rgb888 Turquoise(64, 224, 208);
static constexpr Rgb888 MediumTurquoise(72, 209, 204);
static constexpr Rgb888 DarkTurquoise(0, 206, 209);
static constexpr Rgb888 CadetBlue(95, 158, 160);
static constexpr Rgb888 SteelBlue(70, 130, 180);
static constexpr Rgb888 LightSteelBlue(176, 196, 222);
static constexpr Rgb888 PowderBlue(176, 224, 230);
static constexpr Rgb888 LightBlue(173, 216, 230);
static constexpr Rgb888 SkyBlue(135, 206, 235);
static constexpr Rgb888 LightSkyBlue(135, 206, 250);
static constexpr Rgb888 DeepSkyBlue(0, 191, 255);
static constexpr Rgb888 DodgerBlue(30, 144, 255);
static constexpr Rgb888 CornflowerBlue(100, 149, 237);
static constexpr Rgb888 MediumSlateBlue(123, 104, 238);
static constexpr Rgb888 RoyalBlue(65, 105, 225);
static constexpr Rgb888 Blue(0, 0, 255);
static constexpr Rgb888 MediumBlue(0, 0, 205);
static constexpr Rgb888 DarkBlue(0, 0, 139);
static constexpr Rgb888 Navy(0, 0, 128);
static constexpr Rgb888 MidnightBlue(25, 25, 112);

// Brown HTML Color Names
static constexpr Rgb888 Cornsilk(255, 248, 220);
static constexpr Rgb888 BlanchedAlmond(255, 235, 205);
static constexpr Rgb888 Bisque(255, 228, 196);
static constexpr Rgb888 NavajoWhite(255, 222, 173);
static constexpr Rgb888 Wheat(245, 222, 179);
static constexpr Rgb888 BurlyWood(222, 184, 135);
static constexpr Rgb888 Tan(210, 180, 140);
static constexpr Rgb888 RosyBrown(188, 143, 143);
static constexpr Rgb888 SandyBrown(244, 164, 96);
static constexpr Rgb888 Goldenrod(218, 165, 32);
static constexpr Rgb888 DarkGoldenrod(184, 134, 11);
static constexpr Rgb888 Peru(205, 133, 63);
static constexpr Rgb888 Chocolate(210, 105, 30);
static constexpr Rgb888 SaddleBrown(139, 69, 19);
static constexpr Rgb888 Sienna(160, 82, 45);
static constexpr Rgb888 Brown(165, 42, 42);
static constexpr Rgb888 Maroon(128, 0, 0);

// White HTML Color Names
static constexpr Rgb888 White(255, 255, 255);
static constexpr Rgb888 Snow(255, 250, 250);
static constexpr Rgb888 HoneyDew(240, 255, 240);
static constexpr Rgb888 MintCream(245, 255, 250);
static constexpr Rgb888 Azure(240, 255, 255);
static constexpr Rgb888 AliceBlue(240, 248, 255);
static constexpr Rgb888 GhostWhite(248, 248, 255);
static constexpr Rgb888 WhiteSmoke(245, 245, 245);
static constexpr Rgb888 SeaShell(255, 245, 238);
static constexpr Rgb888 Beige(245, 245, 220);
static constexpr Rgb888 OldLace(253, 245, 230);
static constexpr Rgb888 FloralWhite(255, 250, 230);
static constexpr Rgb888 Ivory(255, 255, 240);
static constexpr Rgb888 AntiqueWhite(250, 235, 215);
static constexpr Rgb888 Linen(250, 240, 230);
static constexpr Rgb888 LavenderBlush(255, 240, 245);
static constexpr Rgb888 MistyRose(255, 228, 225);

// Gray HTML Color Names
static constexpr Rgb888 Gainsboro(220, 220, 220);
static constexpr Rgb888 LightGray(211, 211, 211);
static constexpr Rgb888 Silver(192, 192, 192);
static constexpr Rgb888 DarkGray(169, 169, 169);
static constexpr Rgb888 Gray(128, 128, 128);
static constexpr Rgb888 DimGray(105, 105, 105);
static constexpr Rgb888 LightSlateGray(119, 136, 153);
static constexpr Rgb888 SlateGray(112, 128, 144);
static constexpr Rgb888 DarkSlateGray(47, 79, 79);
static constexpr Rgb888 Black(0, 0, 0);

/// @}

}  // namespace modm::color::html
