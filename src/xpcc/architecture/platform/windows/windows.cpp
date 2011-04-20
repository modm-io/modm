// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#include "../windows.hpp"
#include <xpcc/math/utils/operator.hpp>

// ----------------------------------------------------------------------------
int
snprintf(char *buffer, int buff_size, const char *format, ...)
{
	va_list ap;
	
	// changes e.g. 1.2340e+002 to 1.2340e+02 so that the output is compatible
	// to the GCC output.
	_set_output_format(_TWO_DIGIT_EXPONENT);

	va_start(ap, format);
	int result = vsnprintf(buffer, buff_size, format, ap);
	va_end(ap);
	
	return result;
}

// ----------------------------------------------------------------------------
namespace std
{
	uint16_t
	sqrt(uint16_t x)
	{
		return xpcc::math::sqrt32(x);
	}

	uint16_t
	sqrt(int16_t x)
	{
		return xpcc::math::sqrt32(x);
	}

	uint16_t
	sqrt(uint32_t x)
	{
		return xpcc::math::sqrt32(x);
	}

	uint16_t
	sqrt(int32_t x)
	{
		return xpcc::math::sqrt32(x);
	}

	float
	atan2(int16_t y, int16_t x)
	{
		return std::atan2(static_cast<float>(y), static_cast<float>(x));
	}

	float
	atan2(int32_t y, int32_t x)
	{
		return std::atan2(static_cast<float>(y), static_cast<float>(x));
	}
}
