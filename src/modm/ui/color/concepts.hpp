/*
 * Copyright (c) 2023, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include <bit>

/**
 * @brief		Concepts for colortypes and groups of colortypes.
 * 				Key applications:
 * 				- Color conversion constructors
 * 				- Specialisation of buffer manipulation algorithms
 *
 * @author		Thomas Sommer
 * @ingroup		modm_ui_color
 */
namespace modm::color
{

template <int D>
requires (D > 0)
class Gray;

template<int DR, int DG, int DB>
requires (DR > 0) && (DG > 0) && (DB > 0)
class Rgb;

template<int DR, int DG, int DB>
requires (DR > 0) && (DG > 0) && (DB > 0)
class RgbPallete;

template <int DH, int DS, int DV>
requires (DH > 0) && (DS > 0) && (DV > 0)
class Hsv;

/**
 * @brief 	Identify template class instance
 * @see:	https://stackoverflow.com/questions/44012938/how-to-tell-if-template-type-is-an-instance-of-a-template-class
 */
template <class, template <int...> class>
struct is_instance : public std::false_type {};

template <int...Ts, template <int...> class U>
struct is_instance<U<Ts...>, U> : public std::true_type {};

/**
 * @brief 	Concepts for ColorType templates
 */
template<class C>
concept ColorGray = is_instance<C, Gray>::value;

template<class C>
concept ColorRgb = is_instance<C, Rgb>::value;

template<class C>
concept ColorRgbPallete = is_instance<C, RgbPallete>::value;

template<class C>
concept ColorHsv = is_instance<C, Hsv>::value;

/**
 * @brief 	Concepts for any ColorType
 */
template<class C>
concept Color = ColorGray<C> || ColorRgb<C> || ColorHsv<C> || ColorRgbPallete<C>; // conjunction
// concept Color = std::convertible_to<C, Rgb<8,8,8> >; // more tolerant alternative: convertability

}  // namespace modm::color