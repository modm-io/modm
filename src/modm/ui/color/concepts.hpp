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

#include <bit>

/**
 * @brief			Concepts for colortypes and groups of colortypes.
 * 					Key applications:
 * 					- Color conversion constructors
 * 					- Specialisation of buffer manipulation algorithms
 *
 * @author			Thomas Sommer
 * @ingroup			modm_ui_color
 */
namespace modm::color {

template <int>
class GrayD;

template<int, int, int>
class RgbD;

template<int, int, int>
class RgbStackedD;

template <int, int, int>
class HsvD;

/**
 * @brief 	Identify template class instance
 * @see:	https://stackoverflow.com/questions/44012938/how-to-tell-if-template-type-is-an-instance-of-a-template-class
 */
template <class, template <int...> class>
struct is_instance : public std::false_type {};

template <int...Ts, template <int...> class U>
struct is_instance<U<Ts...>, U> : public std::true_type {};

/**
 * @brief 	Concepts to ident specific colortype instance
 */
template<class C>
concept ColorGray = is_instance<C, GrayD>::value;

template<class C>
concept ColorRgb = is_instance<C, RgbD>::value;

template<class C>
concept ColorRgbStacked = is_instance<C, RgbStackedD>::value;

template<class C>
concept ColorHsv = is_instance<C, HsvD>::value;

/**
 * @brief 	Concept to ident any colortype instance
 */
template<class C>
concept Color = ColorGray<C> || ColorRgb<C> || ColorHsv<C> || ColorRgbStacked<C>; // conjunction
// concept Color = std::convertible_to<C, RgbD<8,8,8> >; // more tolerant alternative: convertability

/**
 * @brief 	Concept to ident palettizing (stacking) colortype instances
 *
 * @see: https://en.wikipedia.org/wiki/Framebuffer#Memory_access
 */
template<class C>
concept ColorPalletized = ColorGray<C> and C::Digits < 8 and std::popcount(unsigned(C::Digits)) == 1;

/**
 * @brief 	Concept to ident non-palettizing (planar) colortype instances
 *
 * @see: https://en.wikipedia.org/wiki/Framebuffer#Memory_access
 */
template<class C>
concept ColorPlanar = !ColorPalletized<C>;

/**
 * @brief 	Concept to ident monochrome colortype
 */
template<class C>
concept ColorMonochrome = std::is_same_v<C, GrayD<1>>;
// concept ColorMonochrome = ColorGray<C> and C::Digits == 1; Alternative implementation as reference

}