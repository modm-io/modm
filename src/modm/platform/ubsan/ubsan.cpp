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

#include <modm/debug/logger.hpp>
#include <cstddef>
#include <cstdint>

#define MODM_UBSAN_RUNTIME __attribute__((no_sanitize("undefined")))

namespace
{

// The data of all handlers starts with the source location of the check
struct SourceLocation
{
	const char* file;
	uint32_t line;
	uint32_t column;
};

constexpr size_t MaxLocations{32};
constinit const SourceLocation* seen[MaxLocations];
constinit bool reporting{false};

MODM_UBSAN_RUNTIME void
report(const char* kind, const SourceLocation* loc)
{
	// The logger itself is instrumented
	if (reporting) return;
	reporting = true;

	size_t index{0};
	for (; index < MaxLocations and seen[index]; index++)
	{
		if (seen[index]->line == loc->line and seen[index]->column == loc->column and
			seen[index]->file == loc->file) break;
	}
	if (index < MaxLocations and not seen[index])
	{
		seen[index] = loc;
		MODM_LOG_ERROR << "UBSAN: " << loc->file << ":" << loc->line << ":"
					   << loc->column << " " << kind << modm::endl;
		if (index + 1 == MaxLocations) {
			MODM_LOG_ERROR << "UBSAN: Too many locations, ignoring new ones!" << modm::endl;
		}
		MODM_LOG_ERROR << modm::flush;
	}
	reporting = false;
}

}

#define MODM_UBSAN_HANDLER(name) \
	extern "C" MODM_UBSAN_RUNTIME void \
	__ubsan_handle_##name(const SourceLocation* loc, ...) \
	{ report(#name, loc); }

// The compiler expects these handlers not to return
#define MODM_UBSAN_HANDLER_NORETURN(name) \
	extern "C" MODM_UBSAN_RUNTIME void \
	__ubsan_handle_##name(const SourceLocation* loc) \
	{ report(#name, loc); __builtin_trap(); }

MODM_UBSAN_HANDLER(divrem_overflow)
MODM_UBSAN_HANDLER(invalid_builtin)
MODM_UBSAN_HANDLER(load_invalid_value)
MODM_UBSAN_HANDLER(out_of_bounds)
MODM_UBSAN_HANDLER(pointer_overflow)
MODM_UBSAN_HANDLER(shift_out_of_bounds)
MODM_UBSAN_HANDLER(vla_bound_not_positive)
MODM_UBSAN_HANDLER_NORETURN(builtin_unreachable)
MODM_UBSAN_HANDLER_NORETURN(missing_return)
