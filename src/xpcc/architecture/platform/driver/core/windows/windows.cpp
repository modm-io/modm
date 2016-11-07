/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012, 2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "windows.hpp"
#include <xpcc/math/utils/operator.hpp>
#include <math.h>

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
		return xpcc::math::sqrt(x);
	}

	uint16_t
	sqrt(int16_t x)
	{
		return xpcc::math::sqrt(x);
	}

	uint16_t
	sqrt(uint32_t x)
	{
		return xpcc::math::sqrt(x);
	}

	uint16_t
	sqrt(int32_t x)
	{
		return xpcc::math::sqrt(x);
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

	bool
	isnan(float f)
	{
		return _isnanf(f);
	}

	bool
	isinf(float f)	{
		return !_finitef(f);
	}
}
