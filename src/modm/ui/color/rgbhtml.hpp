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
static constexpr Rgb IndianRed(205, 92, 92);
static constexpr Rgb LightCoral(240, 128, 128);
static constexpr Rgb Salmon(250, 128, 114);
static constexpr Rgb DarkSalmon(233, 150, 122);
static constexpr Rgb LightSalmon(255, 160, 122);
static constexpr Rgb Crimson(220, 20, 60);
static constexpr Rgb Red(255, 0, 0);
static constexpr Rgb FireBrick(178, 34, 34);
static constexpr Rgb DarkRed(139, 0, 0);

// Pink HTML Color Names
static constexpr Rgb Pink(255, 192, 203);
static constexpr Rgb LightPink(255, 182, 193);
static constexpr Rgb HotPink(255, 105, 180);
static constexpr Rgb DeepPink(255, 20, 147);
static constexpr Rgb MediumVioletRed(199, 21, 133);
static constexpr Rgb PaleVioletRed(219, 112, 147);

// Orange HTML Color Names
static constexpr Rgb Coral(255, 127, 80);
static constexpr Rgb Tomato(255, 99, 71);
static constexpr Rgb OrangeRed(255, 69, 0);
static constexpr Rgb DarkOrange(255, 140, 0);
static constexpr Rgb Orange(25, 165, 0);

// Yellow HTML Color Names
static constexpr Rgb Gold(255, 215, 0);
static constexpr Rgb Yellow(255, 255, 0);
static constexpr Rgb LightYellow(255, 255, 224);
static constexpr Rgb LemonChiffon(255, 250, 205);
static constexpr Rgb LightGoldenrodYellow(250, 250, 210);
static constexpr Rgb PapayaWhip(255, 239, 213);
static constexpr Rgb Moccasin(255, 228, 181);
static constexpr Rgb PeachPuff(255, 218, 185);
static constexpr Rgb PaleGoldenrod(238, 232, 170);
static constexpr Rgb Khaki(240, 230, 140);
static constexpr Rgb DarkKhaki(189, 183, 107);

// Purple HTML Color Names
static constexpr Rgb Lavender(230, 230, 250);
static constexpr Rgb Thistle(216, 191, 216);
static constexpr Rgb Plum(221, 160, 221);
static constexpr Rgb Violet(238, 130, 238);
static constexpr Rgb Orchid(218, 112, 214);
static constexpr Rgb Fuchsia(255, 0, 255);
static constexpr Rgb Magenta(255, 0, 255);
static constexpr Rgb MediumOrchid(186, 85, 211);
static constexpr Rgb MediumPurple(147, 112, 219);
static constexpr Rgb RebeccaPurple(102, 51, 153);
static constexpr Rgb BlueViolet(138, 43, 226);
static constexpr Rgb DarkViolet(148, 0, 211);
static constexpr Rgb DarkOrchid(153, 50, 204);
static constexpr Rgb DarkMagenta(139, 0, 139);
static constexpr Rgb Purple(128, 0, 128);
static constexpr Rgb Indigo(75, 0, 130);
static constexpr Rgb SlateBlue(106, 90, 205);
static constexpr Rgb DarkSlateBlue(72, 61, 139);

// Green HTML Color Names
static constexpr Rgb GreenYellow(173, 255, 47);
static constexpr Rgb Chartreuse(127, 255, 0);
static constexpr Rgb LawnGreen(124, 252, 0);
static constexpr Rgb Lime(0, 255, 0);
static constexpr Rgb LimeGreen(50, 205, 50);
static constexpr Rgb PaleGreen(152, 251, 152);
static constexpr Rgb LightGreen(144, 238, 144);
static constexpr Rgb MediumSpringGreen(0, 250, 154);
static constexpr Rgb SpringGreen(0, 255, 127);
static constexpr Rgb MediumSeaGreen(60, 179, 113);
static constexpr Rgb SeaGreen(46, 139, 87);
static constexpr Rgb ForestGreen(34, 139, 34);
static constexpr Rgb Green(0, 128, 0);
static constexpr Rgb DarkGreen(0, 100, 0);
static constexpr Rgb YellowGreen(154, 205, 50);
static constexpr Rgb OliveDrab(107, 142, 35);
static constexpr Rgb Olive(128, 128, 0);
static constexpr Rgb DarkOliveGreen(85, 107, 47);
static constexpr Rgb MediumAquamarine(102, 205, 170);
static constexpr Rgb DarkSeaGreen(143, 188, 139);
static constexpr Rgb LightSeaGreen(32, 178, 170);
static constexpr Rgb DarkCyan(0, 139, 139);
static constexpr Rgb Teal(0, 128, 128);

// Blue HTML Color Names
static constexpr Rgb Aqua(0, 255, 255);
static constexpr Rgb Cyan(0, 255, 255);
static constexpr Rgb LightCyan(224, 255, 255);
static constexpr Rgb PaleTurquoise(175, 238, 238);
static constexpr Rgb Aquamarine(127, 255, 212);
static constexpr Rgb Turquoise(64, 224, 208);
static constexpr Rgb MediumTurquoise(72, 209, 204);
static constexpr Rgb DarkTurquoise(0, 206, 209);
static constexpr Rgb CadetBlue(95, 158, 160);
static constexpr Rgb SteelBlue(70, 130, 180);
static constexpr Rgb LightSteelBlue(176, 196, 222);
static constexpr Rgb PowderBlue(176, 224, 230);
static constexpr Rgb LightBlue(173, 216, 230);
static constexpr Rgb SkyBlue(135, 206, 235);
static constexpr Rgb LightSkyBlue(135, 206, 250);
static constexpr Rgb DeepSkyBlue(0, 191, 255);
static constexpr Rgb DodgerBlue(30, 144, 255);
static constexpr Rgb CornflowerBlue(100, 149, 237);
static constexpr Rgb MediumSlateBlue(123, 104, 238);
static constexpr Rgb RoyalBlue(65, 105, 225);
static constexpr Rgb Blue(0, 0, 255);
static constexpr Rgb MediumBlue(0, 0, 205);
static constexpr Rgb DarkBlue(0, 0, 139);
static constexpr Rgb Navy(0, 0, 128);
static constexpr Rgb MidnightBlue(25, 25, 112);

// Brown HTML Color Names
static constexpr Rgb Cornsilk(255, 248, 220);
static constexpr Rgb BlanchedAlmond(255, 235, 205);
static constexpr Rgb Bisque(255, 228, 196);
static constexpr Rgb NavajoWhite(255, 222, 173);
static constexpr Rgb Wheat(245, 222, 179);
static constexpr Rgb BurlyWood(222, 184, 135);
static constexpr Rgb Tan(210, 180, 140);
static constexpr Rgb RosyBrown(188, 143, 143);
static constexpr Rgb SandyBrown(244, 164, 96);
static constexpr Rgb Goldenrod(218, 165, 32);
static constexpr Rgb DarkGoldenrod(184, 134, 11);
static constexpr Rgb Peru(205, 133, 63);
static constexpr Rgb Chocolate(210, 105, 30);
static constexpr Rgb SaddleBrown(139, 69, 19);
static constexpr Rgb Sienna(160, 82, 45);
static constexpr Rgb Brown(165, 42, 42);
static constexpr Rgb Maroon(128, 0, 0);

// White HTML Color Names
static constexpr Rgb White(255, 255, 255);
static constexpr Rgb Snow(255, 250, 250);
static constexpr Rgb HoneyDew(240, 255, 240);
static constexpr Rgb MintCream(245, 255, 250);
static constexpr Rgb Azure(240, 255, 255);
static constexpr Rgb AliceBlue(240, 248, 255);
static constexpr Rgb GhostWhite(248, 248, 255);
static constexpr Rgb WhiteSmoke(245, 245, 245);
static constexpr Rgb SeaShell(255, 245, 238);
static constexpr Rgb Beige(245, 245, 220);
static constexpr Rgb OldLace(253, 245, 230);
static constexpr Rgb FloralWhite(255, 250, 230);
static constexpr Rgb Ivory(255, 255, 240);
static constexpr Rgb AntiqueWhite(250, 235, 215);
static constexpr Rgb Linen(250, 240, 230);
static constexpr Rgb LavenderBlush(255, 240, 245);
static constexpr Rgb MistyRose(255, 228, 225);

// Gray HTML Color Names
static constexpr Rgb Gainsboro(220, 220, 220);
static constexpr Rgb LightGray(211, 211, 211);
static constexpr Rgb Silver(192, 192, 192);
static constexpr Rgb DarkGray(169, 169, 169);
static constexpr Rgb Gray(128, 128, 128);
static constexpr Rgb DimGray(105, 105, 105);
static constexpr Rgb LightSlateGray(119, 136, 153);
static constexpr Rgb SlateGray(112, 128, 144);
static constexpr Rgb DarkSlateGray(47, 79, 79);
static constexpr Rgb Black(0, 0, 0);

/// @}

}  // namespace modm::color::html
