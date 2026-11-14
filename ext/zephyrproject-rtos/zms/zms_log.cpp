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

// Forwards the ZMS log to the modm logger, see zephyr/logging/log.h
#include <modm/debug/logger.hpp>
#include <cstdarg>

extern "C" void
zms_log(int level, const char *fmt, ...)
{
	modm::log::Logger &logger = level <= 1 ? modm::log::error : level == 2 ? modm::log::warning :
								level == 3 ? modm::log::info : modm::log::debug;
	logger << "zms: ";
	va_list va;
	va_start(va, fmt);
	logger.vprintf(fmt, va);
	va_end(va);
	logger << modm::endl;
}
