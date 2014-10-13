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

#include <stdarg.h>
#include <stdio.h>		// snprintf()
#include <stdlib.h>

#include "iostream.hpp"

xpcc::IOStream&
xpcc::IOStream::printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	unsigned char c;
	
	// for all chars in format (fmt)
	while ((c = *fmt++) != 0)
	{
		bool isSigned = 0;
		bool isLong = 0;
		bool isLongLong = 0;
		
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
			
			{
				char scratch[26];
				
				ptr = scratch + sizeof(scratch);
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
				while ((c = *ptr++)) {
					this->device->write(c);
				}
			}
		}
	}
	
	va_end(ap);
	return *this;
}

