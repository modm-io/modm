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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__WINDOWS_HPP
#define XPCC__WINDOWS_HPP

/**
 * \internal
 * \file	windows.hpp
 *
 * Provides some defines and functions which are missing in a default
 * windows environment.
 *
 * \ingroup	windows
 */
#include <ciso646>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <limits>

// ----------------------------------------------------------------------------
// WARNINGS & COMPILER SETTINGS

// Disable warnings about array initialization
#pragma warning( disable : 4351 )

// Allow pointer to virtual member functions. May generate non-optimal code but works
// always. See http://msdn.microsoft.com/en-us/library/83cch5a6.aspx
#pragma pointers_to_members( full_generality, virtual_inheritance )

// ----------------------------------------------------------------------------
// CONSTANTS

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef __FLT_MIN__
#define	__FLT_MIN__			1.17549435e-38
#endif

#ifndef __FLT_MAX__
#define	__FLT_MAX__			3.40282e+38
#endif

#ifndef __FLT_EPSILON__
#define	__FLT_EPSILON__		1.19209290e-7
#endif

#ifndef __DBL_MIN__
#define	__DBL_MIN__			2.22507e-308
#endif

#ifndef __DBL_MAX__
#define	__DBL_MAX__			1.79769e+308
#endif

#ifndef __DBL_EPSILON__
#define	__DBL_EPSILON__		2.2204460492503131e-16
#endif

#ifndef NAN
	#define NAN std::numeric_limits<float>::quiet_NaN()
#endif

#ifndef INFINITY
	#define INFINITY std::numeric_limits<float>::infinity()
#endif

// ----------------------------------------------------------------------------
// FUNCTIONS

int
snprintf(char *buffer, int buff_size, const char *format, ...);

inline double
round(double f)
{
	if (f < 0.0) {
		return ceil(f - 0.5);
	}
	else {
		return floor(f + 0.5);
	}
}

namespace std
{
	uint16_t
	sqrt(uint16_t x);

	uint16_t
	sqrt(int16_t x);

	uint16_t
	sqrt(uint32_t x);

	uint16_t
	sqrt(int32_t x);

	float
	atan2(int16_t y, int16_t x);

	float
	atan2(int32_t y, int32_t x);

	bool
	isnan(float f);

	bool
	isinf(float f);
}

#endif // XPCC__WINDOWS_HPP
