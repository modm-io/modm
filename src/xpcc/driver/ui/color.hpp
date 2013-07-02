// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2012, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__COLOR_HPP
#define XPCC__COLOR_HPP

#include <stdint.h>
#include <xpcc/io/iostream.hpp>
#include <xpcc/utils/arithmetic_traits.hpp>
#include <algorithm>

namespace xpcc
{
	namespace color
	{

		template<typename T>
		class HsvT;

		template<typename T>
		class RgbT;

		template<class UnderlyingType = uint8_t>
		class RgbT
		{
		public:
			UnderlyingType red;
			UnderlyingType green;
			UnderlyingType blue;

			template<typename IntermediateType__ = float, unsigned int multiplier__ = 100, typename ReturnType__ = UnderlyingType>
			inline ReturnType__ getRelative(const UnderlyingType color) const
			{
				return static_cast<ReturnType__>(
						(static_cast<IntermediateType__>(color) *
						static_cast<IntermediateType__>(multiplier__)) /
						static_cast<IntermediateType__>(red + green + blue));
			}

			template<typename IntermediateType_ = float, unsigned int multiplier_ = 100, typename ReturnType_ = UnderlyingType>
			inline ReturnType_ getRelativeRed() const
			{
				return getRelative<IntermediateType_, multiplier_, ReturnType_>(red);
			}

			template<typename IntermediateType_ = float, unsigned int multiplier_ = 100, typename ReturnType_ = UnderlyingType>
			inline ReturnType_ getRelativeGreen() const
			{
				return getRelative<IntermediateType_, multiplier_, ReturnType_>(green);
			}

			template<typename IntermediateType_ = float, unsigned int multiplier_ = 100, typename ReturnType_ = UnderlyingType>
			inline ReturnType_ getRelativeBlue() const
			{
				return getRelative<IntermediateType_, multiplier_, ReturnType_>(blue);
			}

			template<typename IntermediateType = float, unsigned int multiplier = 100, typename ReturnType = UnderlyingType>
			inline RgbT<ReturnType> getRelativeColors() const
			{
				return {
					getRelativeRed	<IntermediateType, multiplier, ReturnType>(),
					getRelativeGreen<IntermediateType, multiplier, ReturnType>(),
					getRelativeBlue	<IntermediateType, multiplier, ReturnType>()};
			}

			template<typename T> void
			toHsv(HsvT<T>* color) const;

		private:
			template<typename T>
			friend IOStream&
			operator << ( IOStream& os, const RgbT<T>&);
		};

		typedef RgbT<>	Rgb;

		template<class UnderlyingType = uint8_t>
		class HsvT
		{
		public:
			template<typename T=UnderlyingType>
			void
			toRgb(RgbT<T>* color) const;

			UnderlyingType hue;
			UnderlyingType saturation;
			UnderlyingType value;
		};

		typedef HsvT<>	Hsv;

		template <typename UnderlyingType>
		IOStream& operator << ( IOStream& os, const color::RgbT<UnderlyingType>& color);
	}
}

template <typename UnderlyingType>
xpcc::IOStream&
xpcc::color::operator << ( xpcc::IOStream& os, const xpcc::color::RgbT<UnderlyingType>& color)
{
	os << color.red << "\t" << color.green << "\t" << color.blue;
	return os;
}

/**
 * @see http://de.wikipedia.org/wiki/HSV-Farbraum#Umrechnung_RGB_in_HSV.2FHSL
 * @param color
 */
template<typename UnderlyingType> template<typename T>
inline void xpcc::color::RgbT<UnderlyingType>::toHsv(HsvT<T>* color) const
{
	using namespace std;
	typedef float CalcType;
	const CalcType maxValue = xpcc::ArithmeticTraits<T>::max();
	const CalcType _red		= static_cast<CalcType>(red) / maxValue;
	const CalcType _blue	= static_cast<CalcType>(blue) / maxValue;
	const CalcType _green	= static_cast<CalcType>(green) / maxValue;
	const CalcType _max = max(_red, max(_green, _blue));
	const CalcType _min = min(_red, min(_green, _blue));
	const CalcType _diff = _max - _min;

	CalcType hue_temp;

	// CALCULATE HUE
	if(_max == _min) { // all three color values are the same
		hue_temp = 0;
		color->value = _max * maxValue;
	}
	else if(_max == _red) {
		hue_temp = 60 * (0 + (_green - _blue)	/ _diff );
		color->value = red;
	}
	else if(_max == _green) {
		hue_temp = 60 * (2 + (_blue - _red)		/ _diff );
		color->value = green;
	}
	else /*if(_max == _blue)*/ {
		hue_temp = 60 * (4 + (_red - _green)	/ _diff );
		color->value = blue;
	}

	if(hue_temp < 0) {
		color->hue = (hue_temp + 360	) * (maxValue / 360);
	}
	else {
		color->hue = (hue_temp			) * (maxValue / 360);
	}

	// CALCULATE SATURATION
	if(_max == 0) {
		color->saturation = 0;
	} else {
		color->saturation = _diff / _max * maxValue;
	}
}

#endif // XPCC__COLOR_HPP
