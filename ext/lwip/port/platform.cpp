/*
 * Copyright (c) 2026, Kaelin Laundry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/architecture/interface/assert.hpp>
#include <modm/debug/logger.hpp>

#include <cstdarg>
#include <cstdio>

extern "C" void
modm_lwip_diag(const char* format, ...)
{
	MODM_LOG_DEBUG << "[lwIP] ";
	va_list args;
	va_start(args, format);
	MODM_LOG_DEBUG.vprintf(format, args);
	va_end(args);
	MODM_LOG_DEBUG << modm::endl;
}

extern "C" void
modm_lwip_assert_fail(const char* message, const char* file, int line)
{
	char description[192];
	std::snprintf(description, sizeof(description), "%s at %s:%d", message, file, line);
	modm_assert(false, "lwip.assert", description, line);
}
