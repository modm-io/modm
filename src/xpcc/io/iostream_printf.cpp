// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>		// snprintf()
#include <stdlib.h>

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

		if (c != '%')
		{
			this->device->write(c);
			continue;
		}
		c = *fmt++;

		size_t width = 0;
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
						signedValue = -signedValue; // make it positive
						this->device->write('-');
						if (width) {
							--width;
						}
					}
				}
				unsignedValue = (unsigned long) signedValue;
			}

			writeUnsignedInteger(unsignedValue, base, width, fill);
		}
	}

	return *this;
}

void
xpcc::IOStream::writeUnsignedInteger(
	unsigned long unsignedValue, uint_fast8_t base,
	size_t width, char fill)
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
