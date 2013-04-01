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

namespace xpcc
{
	namespace color
	{
		/**
		 * 24 bit RGB color model.
		 */
		class Rgb
		{
		public:
			uint8_t red;
			uint8_t green;
			uint8_t blue;
		};
		
		/**
		 * 16 bit RGB color model.
		 * 
		 * Uses the 565 model:
		 * - 5 bit red
		 * - 6 bit green
		 * - 5 bit blue
		 */
		class Rgb16
		{
		public:
			static ALWAYS_INLINE Rgb16 white()   { return Rgb16(0xffff); };
			static ALWAYS_INLINE Rgb16 black()   { return Rgb16(0x0000); };
			static ALWAYS_INLINE Rgb16 grey()    { return Rgb16(0xF7DE); };
			static ALWAYS_INLINE Rgb16 blue()    { return Rgb16(0x001F); };
			static ALWAYS_INLINE Rgb16 red()     { return Rgb16(0xF800); };
			static ALWAYS_INLINE Rgb16 magenta() { return Rgb16(0xF81F); };
			static ALWAYS_INLINE Rgb16 green()   { return Rgb16(0x07E0); };
			static ALWAYS_INLINE Rgb16 cyan()    { return Rgb16(0x7FFF); };
			static ALWAYS_INLINE Rgb16 yellow()  { return Rgb16(0xFFE0); };
			
			/**
			 * @param	red
			 * 		Range [0..255]
			 * @param	green
			 * 		Range [0..255]
			 * @param	blue
			 * 		Range [0..255]
			 */
			Rgb16(uint8_t red, uint8_t green, uint8_t blue) :
				value(((static_cast<uint16_t>(red >> 3) << 11) |
						(static_cast<uint16_t>(green >> 2) << 5) |
						static_cast<uint16_t>(blue >> 3)))
			{
			}
			
			Rgb16(uint16_t color) :
				value(color)
			{
			}
			
			inline uint16_t
			getValue() const
			{
				return value;
			}
			
			bool
			operator == (const Rgb16& other) const {
				return (value == other.value);
			}
			
			uint16_t value;
		};
		
		/**
		 * 24 bit HSV (Hue Saturation Value) color space.
		 */
		class Hsv
		{
		public:
			void
			toRgb(Rgb* color);

			uint8_t hue;
			uint8_t saturation;
			uint8_t value;
		};

		template<typename UnderlyingType = uint8_t>
		class Rgba
		{
		public:
			UnderlyingType red;
			UnderlyingType green;
			UnderlyingType blue;
			UnderlyingType alpha;

			template<typename IntermediateType__ = float, unsigned int multiplier__ = 100, typename ReturnType__ = UnderlyingType>
			inline ReturnType__ getRelative(const UnderlyingType color) const
			{
				return static_cast<ReturnType__>(
						(static_cast<IntermediateType__>(color) *
						static_cast<IntermediateType__>(multiplier__)) /
						static_cast<IntermediateType__>(alpha));
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
			inline Rgba<ReturnType> getRelativeColors() const
			{
				return {
					getRelativeRed	<IntermediateType, multiplier, ReturnType>(),
					getRelativeGreen<IntermediateType, multiplier, ReturnType>(),
					getRelativeBlue	<IntermediateType, multiplier, ReturnType>(),
					alpha };
			}
private:
			template<typename T>
			friend IOStream&
			operator << ( IOStream& os, const Rgba<T>&);
		};


		template <typename UnderlyingType>
		IOStream& operator << ( IOStream& os, const color::Rgba<UnderlyingType>& color);
	}
}

template <typename UnderlyingType>
xpcc::IOStream&
xpcc::color::operator << ( xpcc::IOStream& os, const xpcc::color::Rgba<UnderlyingType>& color)
{
	os << color.red << "\t" << color.green << "\t" << color.blue << "\t" << color.alpha;
	return os;
}


#endif // XPCC__COLOR_HPP
