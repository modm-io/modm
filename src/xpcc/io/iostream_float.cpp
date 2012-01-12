// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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

#include <stdio.h>		// snprintf()
#include <stdlib.h>

#include <xpcc/utils/arithmetic_traits.hpp>
#include <xpcc/utils/template_metaprogramming.hpp>

#include "iostream.hpp"

void
xpcc::IOStream::writeFloat(const float& value)
{
	// hard coded for -2.22507e-308
	char str[13 + 1]; // +1 for '\0'
	
#if defined(XPCC__CPU_AVR)
	dtostre(value, str, 5, 0);
	this->device->write(str);
#elif defined(XPCC__CPU_CORTEX_M4)
	float v;
	char *ptr = &str[0];
	
	if (value < 0) {
		v = -value;
		*ptr++ = '-';
	}
	else {
		v = value;
	}
	
	int32_t ep = 0;
	if (v != 0)
	{
		while (v < 1.f) {
			v *= 10;
			ep -= 1;
		}

		if (v >= 10) {
			v *= 0.1f;
			ep += 1;
		}
	}
	
	for (uint32_t i = 0; i < 6; ++i)
	{
		int8_t num = static_cast<int8_t>(v);
		*ptr++ = (num + '0');
		
		if (i == 0) {
			*ptr++ = '.';
		}
		
		// next digit
		v = (v - num) * 10;
	}
	
	*ptr++ = 'e';
	if (ep < 0) {
		ep = -ep;
		*ptr++ = '-';
	}
	else {
		*ptr++ = '+';
	}
	if (ep < 10) {
		*ptr++ = '0';
	}
	*ptr++ = '\0';	// End of string
	this->device->write(str);
	
	this->writeInteger(ep);
#else
	snprintf(str, sizeof(str), "%.5e", value);
	this->device->write(str);
#endif
}

// ----------------------------------------------------------------------------
#if !defined(XPCC__CPU_AVR)
void
xpcc::IOStream::writeDouble(const double& value)
{
#if defined(XPCC__CPU_CORTEX_M4)
	// TODO do this better
	writeFloat(static_cast<float>(value));
#else
	// hard coded for 2.22507e-308
	char str[13 + 1]; // +1 for '\0'
	snprintf(str, sizeof(str), "%.5e", value);
	this->device->write(str);
#endif
}
#endif
