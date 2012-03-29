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

#include "color.hpp"

// ----------------------------------------------------------------------------
void
xpcc::color::Hsv::toRgb(Rgb* color)
{
	uint16_t vs = value * saturation;
	uint16_t h6 = 6 * hue;
	
	uint8_t p = ((value << 8) - vs) >> 8;
	uint8_t i = h6 >> 8;
	uint16_t f = ((i | 1) << 8) - h6;
	if (i & 1) {
		f = -f;
	}
	
	uint8_t u = (((uint32_t) value << 16) - (uint32_t) vs * f) >> 16;
	uint8_t r = value;
	uint8_t g = value;
	uint8_t b = value;
	switch(i)
	{
		case 0: g = u; b = p; break;
		case 1: r = u; b = p; break;
		case 2: r = p; b = u; break;
		case 3: r = p; g = u; break;
		case 4: r = u; g = p; break;
		case 5: g = p; b = u; break;
	}
	
	color->red = r;
	color->green = g;
	color->blue = b;
}
