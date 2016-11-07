/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2014, 2016, Sascha Schade
 * Copyright (c) 2016, Kevin Laeufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>		// snprintf()
#include <stdlib.h>
#include <xpcc/math/utils/misc.hpp>        // xpcc::pow

#include "iostream.hpp"

xpcc::IOStream&
xpcc::IOStream::printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	this->vprintf(fmt, ap);

	va_end(ap);
	return *this;
}

xpcc::IOStream&
xpcc::IOStream::vprintf(const char *fmt, va_list ap)
{
	unsigned char c;

	// for all chars in format (fmt)
	while ((c = *fmt++) != 0)
	{
		bool isSigned = false;
		bool isLong = false;
		bool isLongLong = false;
		bool isFloat = false;
		bool isNegative = false;

		if (c != '%')
		{
			this->device->write(c);
			continue;
		}
		c = *fmt++;

		size_t width = 0;
		size_t width_frac = 0;
		char fill = ' ';
		if (c == '0')
		{
			fill = c;
			c = *fmt++;
		}
		if (c >= '0' && c <= '9')
		{
			width = c - '0';
			c = *fmt++;
		}

		if (c == '.') {
			c = *fmt++;

			if (c >= '0' && c <= '9') {
				width_frac = c - '0';
			}
			c = *fmt++;
		}

		if (c == 'l')
		{
			isLong = true;
			c = *fmt++;
		}
		if (c == 'l')
		{
			isLongLong = true;
			c = *fmt++;
		}

		uint_fast8_t base = 10;
		char *ptr;
		switch (c)
		{
			case 'c':
				c = va_arg(ap, int); // char promoted to int
				/* no break */

			default:
				this->device->write(c);
				continue;

			case 's':
				ptr = (char *) va_arg(ap, char *);
				while ((c = *ptr++))
				{
					this->device->write(c);
				}
				continue;

			case 'f':
				isFloat = true;
				break;

			case 'd':
				isSigned = true;
				/* no break */

			case 'u':
				base = 10;
				break;

			case 'x':
				base = 16;
				break;

			case 'b':
				base = 2;
				break;
		}


		// Number output
		if (isFloat)
		{
			// va_arg(ap, float) not allowed
			float float_value = va_arg(ap, double);

			if (float_value < 0)
			{
				float_value = -float_value; // make it positive
				isNegative = true;
			}

			// Rounding
			float_value += 0.5 / xpcc::pow(10, width_frac);

			// 1) Print integer part
			int width_integer = width - width_frac - 1;
			if (width_integer < 0) {
				width_integer = 0;
			}

			writeUnsignedInteger((unsigned int)float_value, base, width_integer, fill, isNegative);

			// 2) Decimal dot
			this->device->write('.');

			// 3) Fractional part
			float_value = float_value - ((int) float_value);
			float_value *= xpcc::pow(10, width_frac);

			// Alternative: Smaller code size but probably less precise
			// for (uint_fast8_t ii = 0; ii < width_frac; ++ii) {
			//     float_value = float_value * (10.0);
			// }

			// Print fractional part
			writeUnsignedInteger((unsigned int)float_value, base, width_frac, '0', false);
		}
		else
		{
			unsigned long unsignedValue;

			{
				long signedValue = 0;

				if (isLongLong) {
					signedValue = va_arg(ap, long long);
				}
				else if (isLong) {
					signedValue = va_arg(ap, long);
				}
				else {
					signedValue = va_arg(ap, int);
				}

				if (isSigned)
				{
					if (signedValue < 0)
					{
						isNegative = true;
						signedValue = -signedValue; // make it positive
					}
				}
				unsignedValue = (unsigned long) signedValue;
			}

			writeUnsignedInteger(unsignedValue, base, width, fill, isNegative);
		}
	}

	return *this;
}

void
xpcc::IOStream::writeUnsignedInteger(
	unsigned long unsignedValue, uint_fast8_t base,
	size_t width, char fill, bool isNegative)
{
	char scratch[26];

	char *ptr = scratch + sizeof(scratch);
	*--ptr = 0;
	do
	{
		char ch = (unsignedValue % base) + '0';

		if (ch > '9') {
			ch += 'A' - '9' - 1;
		}

		*--ptr = ch;
		unsignedValue /= base;

		if (width) {
			--width;
		}
	} while (unsignedValue);

	// Insert minus sign if needed
	if (isNegative)
	{
		*--ptr = '-';
		if (width) {
			--width;
		}
	}

	// insert padding chars
	while (width--) {
		*--ptr = fill;
	}

	// output result
	char ch;
	while ((ch = *ptr++)) {
		this->device->write(ch);
	}
}
