/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, 2012, Georgi Grinshpun
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2015, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_WINDOWS_HPP
#define MODM_WINDOWS_HPP

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

#endif // MODM_WINDOWS_HPP
