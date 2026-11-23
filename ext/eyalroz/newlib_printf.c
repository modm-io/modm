/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>
#include <printf/printf.h>

// Newlib's strftime calls sniprintf, and asctime and strsignal call siprintf.
// The newlib objects defining them also define snprintf and sprintf, which
// would collide with the functions of this module. Newlib nano does not have
// swprintf, which its wcsftime calls. With LTO, these must be marked as used,
// since the references come from precompiled objects. The linker still removes
// them if nothing calls them.
__attribute__((used))
int sniprintf(char* s, size_t n, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	const int ret = vsnprintf(s, n, format, args);
	va_end(args);
	return ret;
}

__attribute__((used))
int siprintf(char* s, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	const int ret = vsprintf(s, format, args);
	va_end(args);
	return ret;
}

struct wide_output
{
	wchar_t* s;
	size_t n;
	size_t pos;
};

static void put_wide(char c, void* arg)
{
	struct wide_output* out = arg;
	if (out->pos + 1 < out->n) out->s[out->pos] = (unsigned char) c;
	out->pos++;
}

// Only ASCII formats are supported, and neither %ls nor %lc: returns -1.
__attribute__((used))
int swprintf(wchar_t* s, size_t n, const wchar_t* format, ...)
{
	if (n == 0) return -1;
	s[0] = L'\0';

	const size_t length = wcslen(format);
	char narrow[length + 1];
	bool in_conversion = false;
	for (size_t ii = 0; ii <= length; ii++)
	{
		const wchar_t c = format[ii];
		if (c > 0x7f) return -1;
		narrow[ii] = (char) c;
		if (!in_conversion) in_conversion = (c == L'%');
		else if (c == L'%') in_conversion = false;
		else if ((c == L's' || c == L'c') && format[ii - 1] == L'l') return -1;
		else if (c < 0x80 && strchr("diuxXoOcspfFeEgGaAbn", (char) c)) in_conversion = false;
	}

	struct wide_output out = {s, n, 0};
	va_list args;
	va_start(args, format);
	const int ret = vfctprintf(put_wide, &out, narrow, args);
	va_end(args);
	s[out.pos < n ? out.pos : n - 1] = L'\0';
	return (ret < 0 || (size_t) ret >= n) ? -1 : ret;
}
