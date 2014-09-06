// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2014, Roboterclub Aachen e.V.
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
// created with FontCreator 3.0

#include <xpcc/architecture/driver/accessor.hpp>

namespace xpcc
{
	namespace font
	{
		FLASH_STORAGE(uint8_t Matrix8x8[]) =
		{
			0x62, 0x00, // total size of this array
			8,	// width (may vary)
			8,	// height
			0,	// hspace
			0, 	// vspace
			48,	// first char
			10,	// char count
			
			// char widths
			// for each character the separate width in pixels
			 8,  8,  8,  8,  8,  8,  8,  8,  8,  8, 
			
			// font data
			// bit field of all characters
			0x3E, 0x7F, 0x71, 0x59, 0x4D, 0x7F, 0x3E, 0x00, // 48
			0x40, 0x42, 0x7F, 0x7F, 0x40, 0x40, 0x00, 0x00, // 49
			0x62, 0x73, 0x59, 0x49, 0x6F, 0x66, 0x00, 0x00, // 50
			0x22, 0x63, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00, // 51
			0x18, 0x1C, 0x16, 0x53, 0x7F, 0x7F, 0x50, 0x00, // 52
			0x27, 0x67, 0x45, 0x45, 0x7D, 0x39, 0x00, 0x00, // 53
			0x3C, 0x7E, 0x4B, 0x49, 0x79, 0x30, 0x00, 0x00, // 54
			0x03, 0x03, 0x71, 0x79, 0x0F, 0x07, 0x00, 0x00, // 55
			0x36, 0x7F, 0x49, 0x49, 0x7F, 0x36, 0x00, 0x00, // 56
			0x06, 0x4F, 0x49, 0x69, 0x3F, 0x1E, 0x00, 0x00, // 57
		};
	}
}

