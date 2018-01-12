/*
 * Copyright (c) 2014, Kevin Läufer
 * Copyright (c) 2016, Sascha Schade
 * Copyright (c) 2016-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/platform.hpp>
#include <modm/debug/logger.hpp>

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

int
main()
{
	// Let's print some information that is provided in the modm_build_info.hpp
	MODM_LOG_INFO << "Machine:  " << MODM_BUILD_MACHINE  << modm::endl;
	MODM_LOG_INFO << "User:     " << MODM_BUILD_USER     << modm::endl;
	MODM_LOG_INFO << "Os:       " << MODM_BUILD_OS       << modm::endl;
	MODM_LOG_INFO << "Compiler: " << MODM_BUILD_COMPILER << modm::endl;

	MODM_LOG_INFO << "Compare modm's printf and modm with glibc's printf" << modm::endl;

	int32_t ii = -42;

	char format_string[] = ">>%5d<<";

	MODM_LOG_INFO << "modm  ";
	MODM_LOG_INFO.printf(format_string, ii);
	MODM_LOG_INFO << modm::endl;

	char buf[32] = {' '};
	sprintf(buf, format_string, ii);
	MODM_LOG_INFO.printf("glibc %s", buf);
	MODM_LOG_INFO << modm::endl;

	float ff_testvector[] = {123.556789, -123.4};

	for (size_t ii = 0; ii < MODM_ARRAY_SIZE(ff_testvector); ++ii) {
		float ff = ff_testvector[ii];

		for (uint_fast8_t width = 1; width < 10; ++width)
		{
			for (uint_fast8_t width_frac = 1; width_frac < 10; ++width_frac)
			{
				char fmt_str[10];
				sprintf(fmt_str, ">>%%%d.%df<<", width, width_frac);
				MODM_LOG_INFO << "modm  ";
				MODM_LOG_INFO.printf(fmt_str, ff);
				MODM_LOG_INFO << modm::endl;

				char buf[23];
				sprintf(buf, fmt_str, ff);
				MODM_LOG_INFO.printf("glibc %s", buf);

				MODM_LOG_INFO << modm::endl << modm::endl;
			}
		}
	}

	return 0;
}
