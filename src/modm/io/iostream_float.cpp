/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012, 2014, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2016, Tarik TIRE
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdio.h>		// snprintf()
#include <stdlib.h>

#include <modm/utils/arithmetic_traits.hpp>
#include <modm/utils/template_metaprogramming.hpp>

#include "iostream.hpp"

void
modm::IOStream::writeFloat(const float& value)
{
	// hard coded for -2.22507e-308
	char str[13 + 1]; // +1 for '\0'

#if defined(MODM_CPU_AVR)
	dtostre(value, str, 5, 0);
	this->device->write(str);
#elif defined(MODM_CPU_CORTEX_M4) || defined(MODM_CPU_CORTEX_M3) || defined(MODM_CPU_CORTEX_M0) || defined(MODM_OS_WIN32)
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

		while (v > 10) {
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
	snprintf(str, sizeof(str), "%.5e", (double) value);
	this->device->write(str);
#endif
}

// ----------------------------------------------------------------------------
#if !defined(MODM_CPU_AVR)
void
modm::IOStream::writeDouble(const double& value)
{
#if defined(MODM_CPU_CORTEX_M4) || defined(MODM_CPU_CORTEX_M3) || defined(MODM_CPU_CORTEX_M0) || defined(MODM_OS_WIN32)
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
